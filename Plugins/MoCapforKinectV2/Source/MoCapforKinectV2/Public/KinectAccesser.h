#ifndef __KINECTACCESSER
#define __KINECTACCESSER

#include "MoCapforKinectV2.h"
#include "Stats.h"
#include "Tickable.h"

class MOCAPFORKINECTV2_API FKinectAccesser : public FTickableGameObject
{
protected:
	// Constant for matching units (Kinect camera space [unit = m] -> UE4 world space [unit = cm])
	const float unitDifference = 100;

	// Kinect.Face.dll
	HMODULE hModule;

	// Kinect interfaces
	IKinectSensor* pSensor = nullptr;
	IBodyFrameReader* pBodyFrameReader = nullptr;
	ICoordinateMapper* pCoordinateMapper = nullptr;
	// Kinect face interfaces
	IFaceFrameSource* pFaceFrameSources[BODY_COUNT] = { nullptr };
	IFaceFrameReader* pFaceFrameReaders[BODY_COUNT] = { nullptr };
	// Body data
	IBody* pBodies[BODY_COUNT] = { nullptr };

	// Body tracking states
	bool mIsBodyTrackings[BODY_COUNT] = { false };
	// Face tracking states
	bool mIsFaceTrackings[BODY_COUNT] = { false };
	// Joint coordinates
	Joint mJoints[BODY_COUNT][JointType_Count] = { {JointType_SpineBase, {0.f, 0.f, 0.f}, TrackingState_NotTracked } };
	// Joint orientation
	JointOrientation mJointOrientations[BODY_COUNT][JointType_Count] = { {JointType_SpineBase, {0.f, 0.f, 0.f, 0.f} } };
	// Both hand states
	HandState mLeftHandStates[BODY_COUNT] = { HandState_Unknown };
	HandState mRightHandStates[BODY_COUNT] = { HandState_Unknown };
	// Face properties
	DetectionResult mFaceProperties[BODY_COUNT][FaceProperty_Count] = { DetectionResult_Unknown };
	// Head orientation
	Vector4 mFaceRotation[BODY_COUNT] = { {0.f, 0.f, 0.f, 0.f} };

public:
	// Was Kinect initialized successfully
	bool bIsKinectSensorInitialized = false;
	// Are the features of Kinect used
	bool bIsKinectSensorEnable = false;

public:
	// Initialize Kinect sensor and get each interface
	bool InitializeKinectSensor();

	// Close Kinect sensor and release each interface
	void CloseKinectSensor();

	// Tick Event
	virtual TStatId GetStatId() const { RETURN_QUICK_DECLARE_CYCLE_STAT(FKinectAccesser, STATGROUP_Tickables); }
	virtual bool IsTickable() const { return true; }
	virtual void Tick(float DeltaTime);

	// Get the state of both hands 
	void GetBothHandsState(int BodyIndex, EHandState& LeftHandState, EHandState& RightHandState);
	
	// Get body tracking status
	bool GetBodyTrackingState(int BodyIndex);

	// Get face tracking states
	bool GetFaceTrackingState(int BodyIndex);
	
	// Get tracking status of each joint
	TArray<ETrackingState> GetJointTrackingState(int BodyIndex);
	
	// Get coordinates of each joint
	TArray<FVector> GetJointLocation(int BodyIndex, AActor* Owner);
	
	// Get rotation of each joint (Doesn't include torsional rotation infomation)
	TArray<FRotator> GetJointRotation(int BodyIndex, AActor* Owner);

	// Get location of root joint
	FVector GetRootLocation(int BodyIndex, AActor* Owner);

	// Get waist twist rotation
	FRotator GetWaistTwist(int BodyIndex, AActor* Owner);

	// Get face property
	TArray<EFaceResult> GetFaceProperty(int BodyIndex);

	// Get head orientation
	FRotator GetFaceRotation(int BodyIndex, AActor* Owner);

	// Get Vaild Body Index
	int GetVaildBodyIndex();

protected:
	// Get skeletal infomation and hand status
	void ProcessBody(int nBodyCount, IBody** ppBodies);

	// Get face properties
	void ProcessFace(int nBodyCount, IBody** ppBodies);

private:
	// Convert JointType to EJointType
	EJointType ToEJointType(JointType type);
	
	// Convert HandState to EHandState
	EHandState ToEHandState(HandState State);
	
	// Convert TrakcingState to ETrackingState
	ETrackingState ToETrackingState(TrackingState State);

	// Convert DetectionResult to EFaceResult
	EFaceResult ToEFaceResult(DetectionResult State);

/////////////////////////////////////
// Processing for singleton class
protected:
	// One and only existence
	static FKinectAccesser* pInstance;
	
	// Constructor
	FKinectAccesser();

	// Destructor
	~FKinectAccesser();

public:
	// Create instance of this class
	static void Create();
	
	// Destory instance of this class
	static void Destroy();
	
	// Get instance of this class
	static FKinectAccesser* GetInstance() { return pInstance; }
};

#endif // !__KINECTACCESSER
