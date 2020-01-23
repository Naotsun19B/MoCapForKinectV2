#include "KinectAccesser.h"
#include "KinectBPLibrary.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"

typedef HRESULT(*FACE)(_In_ IKinectSensor* sensor, _In_ const UINT64 initialTrackingId, _In_ const DWORD initialFaceFrameFeatures, _COM_Outptr_ IFaceFrameSource** ppSource);

//安全にポインタを解放する関数
template<class Interface>
inline void SafeRelease(Interface*& pInterfaceToRelease)
{
	if (pInterfaceToRelease != NULL)
	{
		pInterfaceToRelease->Release();
		pInterfaceToRelease = NULL;
	}
}

FKinectAccesser* FKinectAccesser::pInstance = NULL;

void FKinectAccesser::Create()
{
	if (!pInstance)
	{
		pInstance = new FKinectAccesser();
	}
}

void FKinectAccesser::Destroy()
{
	delete pInstance;
	pInstance = NULL;
}

FKinectAccesser::FKinectAccesser() : FTickableGameObject()
{
	FString dllPath = FMoCapforKinectV2Module::GetKinectFaceDir() + TEXT("Kinect20.Face.dll");
	hModule = LoadLibrary((LPCWSTR)*dllPath);
	if (!hModule)
	{
		UE_LOG(MoCapForKinectV2, Fatal, TEXT("Can't load dll : %s"), *dllPath);
	}
}

FKinectAccesser::~FKinectAccesser()
{
	if (hModule)
	{
		FreeModule(hModule);
	}

	if (bIsKinectSensorInitialized)
	{
		CloseKinectSensor();
	}
}

bool FKinectAccesser::InitializeKinectSensor()
{
	if (bIsKinectSensorInitialized) 
	{
		UE_LOG(MoCapForKinectV2, Log, TEXT("KinectSensor is already initialized"));
		return false;
	}

	HRESULT hr;

	//デフォルトのKinectを取得する
	hr = GetDefaultKinectSensor(&pSensor);
	if (pSensor && SUCCEEDED(hr))
	{
		hr = pSensor->Open();
	}

	//CoordinateMapperを取得
	if (SUCCEEDED(hr))
	{
		hr = pSensor->get_CoordinateMapper(&pCoordinateMapper);
	}

	//ボディリーダーを取得する
	IBodyFrameSource* pBodyFrameSource = nullptr;
	if (SUCCEEDED(hr))
	{
		hr = pSensor->get_BodyFrameSource(&pBodyFrameSource);
	}
	if (SUCCEEDED(hr))
	{
		hr = pBodyFrameSource->OpenReader(&pBodyFrameReader);
	}

	//ボディ配列を初期化
	for (auto& body : pBodies) body = nullptr;

	//CreateFaceFrameSourceの関数アドレスを取得
	FACE pCreateFaceFrameSource = (FACE)GetProcAddress(hModule, "CreateFaceFrameSource");
	if (!pCreateFaceFrameSource)
	{
		UE_LOG(MoCapForKinectV2, Fatal, TEXT("Can't get address"));
	}

	//表情認識に必要なインターフェースを取得
	const DWORD c_FaceFrameFeatures =
		FaceFrameFeatures::FaceFrameFeatures_BoundingBoxInColorSpace
		| FaceFrameFeatures::FaceFrameFeatures_PointsInColorSpace
		| FaceFrameFeatures::FaceFrameFeatures_RotationOrientation
		| FaceFrameFeatures::FaceFrameFeatures_Happy
		| FaceFrameFeatures::FaceFrameFeatures_RightEyeClosed
		| FaceFrameFeatures::FaceFrameFeatures_LeftEyeClosed
		| FaceFrameFeatures::FaceFrameFeatures_MouthOpen
		| FaceFrameFeatures::FaceFrameFeatures_MouthMoved
		| FaceFrameFeatures::FaceFrameFeatures_LookingAway
		| FaceFrameFeatures::FaceFrameFeatures_Glasses
		| FaceFrameFeatures::FaceFrameFeatures_FaceEngagement;

	if (SUCCEEDED(hr))
	{
		for (int i = 0; i < BODY_COUNT; i++)
		{
			if (SUCCEEDED(hr))
			{
				pCreateFaceFrameSource(pSensor, 0, c_FaceFrameFeatures, &pFaceFrameSources[i]);
			}
			if (SUCCEEDED(hr))
			{
				pFaceFrameSources[i]->OpenReader(&pFaceFrameReaders[i]);
			}
		}
	}

	SafeRelease(pBodyFrameSource);

	if (SUCCEEDED(hr))
	{
		UE_LOG(MoCapForKinectV2, Log, TEXT("KinectSensor is initialized"));
		bIsKinectSensorInitialized = true;
		bIsKinectSensorEnable = true;
	}
	else
	{
		return false;
	}

	return true;
}

void FKinectAccesser::CloseKinectSensor()
{
	for (int iFace = 0; iFace < BODY_COUNT; iFace++)
	{
		SafeRelease(pFaceFrameReaders[iFace]);
		SafeRelease(pFaceFrameSources[iFace]);
		SafeRelease(pBodies[iFace]);
	}
	SafeRelease(pCoordinateMapper);
	SafeRelease(pBodyFrameReader);

	if (pSensor)
	{
		pSensor->Close();
	}

	bIsKinectSensorInitialized = false;
	bIsKinectSensorEnable = false;
}

void FKinectAccesser::Tick(float DeltaTime)
{
	if (!bIsKinectSensorEnable)
	{
		return;
	}

	if (!bIsKinectSensorInitialized)
	{
		UE_LOG(MoCapForKinectV2, Log, TEXT("Kinect sensor is not initialized"));
		return;
	}

	if (!pBodyFrameReader)
	{
		UE_LOG(MoCapForKinectV2, Log, TEXT("BodyFrameReader is not found"));
		return;
	}

	IBodyFrame* pBodyFrame = NULL;

	HRESULT hr = pBodyFrameReader->AcquireLatestFrame(&pBodyFrame);

	if (SUCCEEDED(hr))
	{
		IBody* ppBodies[BODY_COUNT] = { 0 };

		pBodyFrame->GetAndRefreshBodyData(_countof(ppBodies), ppBodies);

		ProcessBody(BODY_COUNT, ppBodies);
		ProcessFace(BODY_COUNT, ppBodies);

		for (int i = 0; i < _countof(ppBodies); ++i)
		{
			SafeRelease(ppBodies[i]);
		}
	}

	SafeRelease(pBodyFrame);
}

void FKinectAccesser::ProcessBody(int nBodyCount, IBody** ppBodies)
{
	if (pCoordinateMapper)
	{
		for (int i = 0; i < nBodyCount; i++)
		{
			mIsBodyTrackings[i] = false;
		}

		for (int i = 0; i < nBodyCount; ++i)
		{
			IBody* pBody = ppBodies[i];
			if (pBody)
			{
				BOOLEAN bTracked = false;
				HRESULT hr = pBody->get_IsTracked(&bTracked);

				if (SUCCEEDED(hr) && bTracked)
				{
					mIsBodyTrackings[i] = true;

					pBody->get_HandLeftState(&mLeftHandStates[i]);
					pBody->get_HandRightState(&mRightHandStates[i]);

					pBody->GetJoints(_countof(mJoints[i]), mJoints[i]);
					pBody->GetJointOrientations(_countof(mJointOrientations[i]), mJointOrientations[i]);
				}
			}
		}
	}
}

void FKinectAccesser::ProcessFace(int nBodyCount, IBody** ppBodies)
{
	HRESULT hr;

	for (int iFace = 0; iFace < nBodyCount; ++iFace)
	{
		mIsFaceTrackings[iFace] = false;
		IFaceFrame* pFaceFrame = nullptr;
		hr = pFaceFrameReaders[iFace]->AcquireLatestFrame(&pFaceFrame);

		BOOLEAN bFaceTracked = false;
		if (SUCCEEDED(hr) && nullptr != pFaceFrame)
		{
			hr = pFaceFrame->get_IsTrackingIdValid(&bFaceTracked);
		}

		if (SUCCEEDED(hr))
		{
			if (bFaceTracked)
			{
				mIsFaceTrackings[iFace] = true;

				IFaceFrameResult* pFaceFrameResult = nullptr;
				RectI faceBox = { 0 };
				PointF facePoints[FacePointType::FacePointType_Count];

				hr = pFaceFrame->get_FaceFrameResult(&pFaceFrameResult);

				if (SUCCEEDED(hr) && pFaceFrameResult != nullptr)
				{
					hr = pFaceFrameResult->get_FaceBoundingBoxInColorSpace(&faceBox);

					if (SUCCEEDED(hr))
					{
						hr = pFaceFrameResult->GetFacePointsInColorSpace(FacePointType::FacePointType_Count, facePoints);
					}

					if (SUCCEEDED(hr))
					{
						hr = pFaceFrameResult->get_FaceRotationQuaternion(&mFaceRotation[iFace]);
					}

					if (SUCCEEDED(hr))
					{
						hr = pFaceFrameResult->GetFaceProperties(FaceProperty::FaceProperty_Count, mFaceProperties[iFace]);
					}
				}

				SafeRelease(pFaceFrameResult);
			}
			else
			{
				IBody* pBody = ppBodies[iFace];
				if (pBody != nullptr)
				{
					BOOLEAN bTracked = false;
					hr = pBody->get_IsTracked(&bTracked);

					UINT64 bodyTId;
					if (SUCCEEDED(hr) && bTracked)
					{
						hr = pBody->get_TrackingId(&bodyTId);
						if (SUCCEEDED(hr))
						{
							pFaceFrameSources[iFace]->put_TrackingId(bodyTId);
						}
					}
				}
			}
		}

		SafeRelease(pFaceFrame);
	}

	for (int i = 0; i < nBodyCount; ++i)
	{
		SafeRelease(ppBodies[i]);
	}
}

void FKinectAccesser::GetBothHandsState(int BodyIndex, EHandState& LeftHandState, EHandState& RightHandState)
{
	LeftHandState = ToEHandState(mLeftHandStates[BodyIndex]);
	RightHandState = ToEHandState(mRightHandStates[BodyIndex]);
}

bool FKinectAccesser::GetBodyTrackingState(int BodyIndex)
{
	return mIsBodyTrackings[BodyIndex];
}

bool FKinectAccesser::GetFaceTrackingState(int BodyIndex)
{
	return mIsFaceTrackings[BodyIndex];
}

TArray<ETrackingState> FKinectAccesser::GetJointTrackingState(int BodyIndex)
{
	TArray<ETrackingState> buffer;
	for (int jtc = 0; jtc < JointType::JointType_Count; jtc++)
	{
		buffer.Add(ToETrackingState(mJoints[BodyIndex][jtc].TrackingState));
	}
	 
	return buffer;
}

TArray<FVector> FKinectAccesser::GetJointLocation(int BodyIndex, AActor* Owner)
{
	TArray<FVector> buffer;
	for (int jtc = 0; jtc < JointType::JointType_Count; jtc++)
	{
		FVector location;

		// <Kinect Camera Space Coodinate>
		// X : Kinect left		(User right)
		// Y : Kinect upward	(User upward)
		// Z : Kinect forward	(User backward)
		// --------------------------------------
		// <UE4 World Coodinate>
		// X : Character forward
		// Y : Character right
		// Z : Character upward

		location.X = mJoints[BodyIndex][jtc].Position.Z * unitDifference * -1;
		location.Y = mJoints[BodyIndex][jtc].Position.X * unitDifference;
		location.Z = mJoints[BodyIndex][jtc].Position.Y * unitDifference;

		location = UKismetMathLibrary::TransformLocation(Owner->GetActorTransform(), location);

		buffer.Add(Owner->GetActorLocation() + location);
	}

	return buffer;
}

TArray<FRotator> FKinectAccesser::GetJointRotation(int BodyIndex, AActor* Owner)
{
	TArray<FVector> locations = GetJointLocation(BodyIndex, Owner);
	TArray<FRotator> buffer;
	buffer.Empty();
	
	for (int jtc = 0; jtc < JointType_Count; jtc++)
	{
		FRotator lookAtRot = { 0,0,0 };
		EJointType childJoint = UKinectBPLibrary::GetChildBoneOfKinectSkeleton((EJointType)jtc);
		if (childJoint == EJointType::JT_Unknown)
		{
			EJointType parentJoint = UKinectBPLibrary::GetParentBoneOfKinectSkeleton((EJointType)jtc);
			lookAtRot = UKismetMathLibrary::FindLookAtRotation(locations[(int)parentJoint], locations[jtc]);
		}
		else
		{
			lookAtRot = UKismetMathLibrary::FindLookAtRotation(locations[jtc], locations[(int)childJoint]);
		}

		buffer.Add(lookAtRot);
	}

	return buffer;
}

FVector FKinectAccesser::GetRootLocation(int BodyIndex, AActor* Owner)
{
	FVector buffer;

	buffer.X = mJoints[BodyIndex][JointType::JointType_SpineBase].Position.Z * unitDifference * -1;
	buffer.Y = mJoints[BodyIndex][JointType::JointType_SpineBase].Position.X * unitDifference;
	buffer.Z = mJoints[BodyIndex][JointType::JointType_SpineBase].Position.Y * unitDifference;

	buffer = UKismetMathLibrary::TransformLocation(Owner->GetActorTransform(), buffer);

	return Owner->GetActorLocation() + buffer;
}

FRotator FKinectAccesser::GetWaistTwist(int BodyIndex, AActor* Owner)
{
	TArray<FVector> locations = GetJointLocation(BodyIndex, Owner);	
	float leftShoulder = mJoints[BodyIndex][JointType::JointType_ShoulderLeft].Position.Z;
	float rightShoulder = mJoints[BodyIndex][JointType::JointType_ShoulderRight].Position.Z;

	if (
		FMath::Abs(leftShoulder - rightShoulder) < 0.1
	)
	{
		return UKismetMathLibrary::FindLookAtRotation(
			FVector(
				locations[JointType::JointType_ShoulderRight].X,
				locations[JointType::JointType_ShoulderRight].Y,
				0.f
			),
			FVector(
				locations[JointType::JointType_ShoulderLeft].X,
				locations[JointType::JointType_ShoulderLeft].Y,
				0.f
			)
		);
	}
	else if (leftShoulder < rightShoulder)
	{
		return UKismetMathLibrary::FindLookAtRotation(
			FVector(
				locations[JointType::JointType_SpineShoulder].X,
				locations[JointType::JointType_SpineShoulder].Y,
				0.f
			),
			FVector(
				locations[JointType::JointType_ShoulderLeft].X,
				locations[JointType::JointType_ShoulderLeft].Y,
				0.f
			)
		);
	}
	else if (rightShoulder < leftShoulder)
	{
		return UKismetMathLibrary::FindLookAtRotation(
			FVector(
				locations[JointType::JointType_SpineShoulder].X,
				locations[JointType::JointType_SpineShoulder].Y,
				0.f
			),
			FVector(
				locations[JointType::JointType_ShoulderRight].X,
				locations[JointType::JointType_ShoulderRight].Y,
				0.f
			)
		) + FRotator(0.f, 180.f, 0.f);
	}
	else
	{
		return FRotator(0.f, 180.f, 0.f);
	}
}

TArray<EFaceResult> FKinectAccesser::GetFaceProperty(int BodyIndex)
{
	TArray<EFaceResult> properties;
	for (int i = 0; i < FaceProperty::FaceProperty_Count; i++)
	{
		properties.Add(ToEFaceResult(mFaceProperties[BodyIndex][i]));
	}

	return properties;
}

FRotator FKinectAccesser::GetFaceRotation(int BodyIndex, AActor* Owner)
{
	FQuat rawData = { mFaceRotation[BodyIndex].x, mFaceRotation[BodyIndex].y, mFaceRotation[BodyIndex].z, mFaceRotation[BodyIndex].w };
	FRotator source = rawData.Rotator();
	FRotator result;

	//座標軸を変換
	result.Roll		= source.Yaw * -1;
	result.Pitch	= source.Roll * -1;
	result.Yaw		= source.Pitch;

	return Owner->GetActorRotation() + result;
}

int FKinectAccesser::GetVaildBodyIndex()
{
	for (int i = 0; i < BODY_COUNT; i++)
	{
		if (GetBodyTrackingState(i))
		{
			return i;
		}
	}

	return -1;
}

EJointType FKinectAccesser::ToEJointType(JointType type)
{
	switch (type)
	{
	case JointType::JointType_SpineBase:		return EJointType::JT_SpineBase;
	case JointType::JointType_SpineMid:			return EJointType::JT_SpineMid;
	case JointType::JointType_Neck:				return EJointType::JT_Neck;
	case JointType::JointType_Head:				return EJointType::JT_Head;
	case JointType::JointType_ShoulderLeft:		return EJointType::JT_ShoulderLeft;
	case JointType::JointType_ElbowLeft:		return EJointType::JT_ElbowLeft;
	case JointType::JointType_WristLeft:		return EJointType::JT_WristLeft;
	case JointType::JointType_HandLeft:			return EJointType::JT_HandLeft;
	case JointType::JointType_ShoulderRight:	return EJointType::JT_ShoulderRight;
	case JointType::JointType_ElbowRight:		return EJointType::JT_ElbowRight;
	case JointType::JointType_WristRight:		return EJointType::JT_WristRight;
	case JointType::JointType_HandRight:		return EJointType::JT_HandRight;
	case JointType::JointType_HipLeft:			return EJointType::JT_HipLeft;
	case JointType::JointType_KneeLeft:			return EJointType::JT_KneeLeft;
	case JointType::JointType_AnkleLeft:		return EJointType::JT_AnkleLeft;
	case JointType::JointType_FootLeft:			return EJointType::JT_FootLeft;
	case JointType::JointType_HipRight:			return EJointType::JT_HipRight;
	case JointType::JointType_KneeRight:		return EJointType::JT_KneeRight;
	case JointType::JointType_AnkleRight:		return EJointType::JT_AnkleRight;
	case JointType::JointType_FootRight:		return EJointType::JT_FootRight;
	case JointType::JointType_SpineShoulder:	return EJointType::JT_SpineShoulder;
	case JointType::JointType_HandTipLeft:		return EJointType::JT_HandTipLeft;
	case JointType::JointType_ThumbLeft:		return EJointType::JT_ThumbLeft;
	case JointType::JointType_HandTipRight:		return EJointType::JT_HandTipRight;
	case JointType::JointType_ThumbRight:		return EJointType::JT_ThumbRight;
	default:									return EJointType::JT_Unknown;
	}
}

EHandState FKinectAccesser::ToEHandState(HandState State)
{
	switch (State)
	{
	case HandState::HandState_Unknown:		return EHandState::HS_Unknown;
	case HandState::HandState_NotTracked:	return EHandState::HS_NotTracked;
	case HandState::HandState_Open:			return EHandState::HS_Open;
	case HandState::HandState_Closed:		return EHandState::HS_Closed;
	case HandState::HandState_Lasso:		return EHandState::HS_Lasso;
	default:								return EHandState::HS_Unknown;
	}
}

ETrackingState FKinectAccesser::ToETrackingState(TrackingState State)
{
	switch (State)
	{
	case TrackingState::TrackingState_NotTracked:	return ETrackingState::TS_NotTracked;
	case TrackingState::TrackingState_Inferred:		return ETrackingState::TS_Inferred;
	case TrackingState::TrackingState_Tracked:		return ETrackingState::TS_Tracked;
	default:										return ETrackingState::TS_NotTracked;
	}
}

EFaceResult FKinectAccesser::ToEFaceResult(DetectionResult State)
{
	switch (State)
	{
	case DetectionResult::DetectionResult_Unknown:	return EFaceResult::FR_Unknown;
	case DetectionResult::DetectionResult_No:		return EFaceResult::FR_No;
	case DetectionResult::DetectionResult_Maybe:	return EFaceResult::FR_Maybe;
	case DetectionResult::DetectionResult_Yes:		return EFaceResult::FR_Yes;
	default:										return EFaceResult::FR_Unknown;
	}
}
