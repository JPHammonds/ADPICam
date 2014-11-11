/* PICam.cpp */
#include "ADPICam.h"
#include <string>

#define MAX_ENUM_STATES 16

/** Configuration command for PICAM driver; creates a new PICam object.
 * \param[in] portName The name of the asyn port driver to be created.
 * \param[in] maxBuffers The maximum number of NDArray buffers that the NDArrayPool for this driver is
 *            allowed to allocate. Set this to -1 to allow an unlimited number of buffers.
 * \param[in] maxMemory The maximum amount of memory that the NDArrayPool for this driver is
 *            allowed to allocate. Set this to -1 to allow an unlimited amount of memory.
 * \param[in] priority The thread priority for the asyn port driver thread if ASYN_CANBLOCK is set in asynFlags.
 * \param[in] stackSize The stack size for the asyn port driver thread if ASYN_CANBLOCK is set in asynFlags.
 */
extern "C" int PICamConfig(const char *portName, int maxBuffers,
		size_t maxMemory, int priority, int stackSize) {
	new ADPICam(portName, maxBuffers, maxMemory, priority, stackSize);
	return (asynSuccess);
}

/** Configuration command for PICAM driver; creates a new PICam object.
 * \param[in]  demoCameraName String identifying demoCameraName
 */
extern "C" int PICamAddDemoCamera(const char *demoCameraName) {
	ADPICam::piAddDemoCamera(demoCameraName);
	return (asynSuccess);
}

ADPICam * ADPICam::ADPICam_Instance = NULL;
const char *ADPICam::notAvailable = "N/A";
const char *ADPICam::driverName = "PICam";

ADPICam::ADPICam(const char *portName, int maxBuffers, size_t maxMemory,
		int priority, int stackSize) :
		ADDriver(portName, 1, int(NUM_PICAM_PARAMS), maxBuffers, maxMemory,
		asynEnumMask, asynEnumMask, ASYN_CANBLOCK, 1, priority, stackSize) {
	int status = asynSuccess;
	static const char *functionName = "PICam";
	pibln libInitialized;
	PicamCameraID demoId;
	PicamError error = PicamError_None;

	currentCameraHandle = NULL;
	selectedCameraIndex = -1;
	availableCamerasCount = 0;
	unavailableCamerasCount = 0;
	Picam_InitializeLibrary();
	Picam_IsLibraryInitialized(&libInitialized);
	ADPICam_Instance = this;
	if (!libInitialized) {
		asynPrint(pasynUserSelf, ASYN_TRACE_FLOW,
				"%s%s Trouble Initializing Picam Library", driverName,
				functionName);
	}
	Picam_ConnectDemoCamera(PicamModel_Pixis100F, "0008675309", &demoId);
	Picam_OpenFirstCamera(&currentCameraHandle);
	selectedCameraIndex = 0;
	lock();
	createParam(PICAM_VersionNumberString, asynParamOctet,
			&PICAM_VersionNumber);
	// Available Camera List
	createParam(PICAM_AvailableCamerasString, asynParamInt32,
			&PICAM_AvailableCameras);
	createParam(PICAM_CameraInterfaceString, asynParamOctet,
			&PICAM_CameraInterface);
	createParam(PICAM_SensorNameString, asynParamOctet, &PICAM_SensorName);
	createParam(PICAM_SerialNumberString, asynParamOctet, &PICAM_SerialNumber);
	createParam(PICAM_FirmwareRevisionString, asynParamOctet,
			&PICAM_FirmwareRevision);
	//Unavailable Camera List
	createParam(PICAM_UnavailableCamerasString, asynParamInt32,
			&PICAM_UnavailableCameras);
	createParam(PICAM_CameraInterfaceUnavailableString, asynParamOctet,
			&PICAM_CameraInterfaceUnavailable);
	createParam(PICAM_SensorNameUnavailableString, asynParamOctet,
			&PICAM_SensorNameUnavailable);
	createParam(PICAM_SerialNumberUnavailableString, asynParamOctet,
			&PICAM_SerialNumberUnavailable);
	createParam(PICAM_FirmwareRevisionUnavailableString, asynParamOctet,
			&PICAM_FirmwareRevisionUnavailable);
	//Analog to Digital Conversion
	createParam(PICAM_AdcAnalogGainString, asynParamInt32,
			&PICAM_AdcAnalogGain);
	createParam(PICAM_AdcBitDepthString, asynParamInt32,
			&PICAM_AdcBitDepth);
	createParam(PICAM_AdcEMGainString, asynParamInt32,
			&PICAM_AdcEMGain);
	createParam(PICAM_AdcQualityString, asynParamInt32,
			&PICAM_AdcQuality);
	createParam(PICAM_AdcSpeedString, asynParamInt32,
			&PICAM_AdcSpeed);
	createParam(PICAM_CorrectPixelBiasString, asynParamInt32,
			&PICAM_CorrectPixelBias);
	// Readout calcs
	createParam(PICAM_ReadoutTimeCalcString, asynParamFloat64,
			&PICAM_ReadoutTimeCalc);
	createParam(PICAM_ReadoutRateCalcString, asynParamFloat64,
			&PICAM_ReadoutRateCalc);
	createParam(PICAM_FrameRateCalcString, asynParamFloat64,
			&PICAM_FrameRateCalc);
	createParam(PICAM_OnlineReadoutRateCalcString, asynParamFloat64,
			&PICAM_OnlineReadoutRateCalc);

	// Camera Parameter Relevance
	createParam(PICAM_ExposureTimeRelString, asynParamInt32,
			&PICAM_ExposureTimeRelevant);
	createParam(PICAM_ShutterClosingDelayRelString, asynParamInt32,
			&PICAM_ShutterClosingDelayRelevant);
	createParam(PICAM_ShutterDelayResolutionRelString, asynParamInt32,
			&PICAM_ShutterDelayResolutionRelevant);
	createParam(PICAM_ShutterOpeningDelayRelString, asynParamInt32,
			&PICAM_ShutterOpeningDelayRelevant);
	createParam(PICAM_ShutterTimingModeRelString, asynParamInt32,
			&PICAM_ShutterTimingModeRelevant);
	createParam(PICAM_BracketGatingRelString, asynParamInt32,
			&PICAM_BracketGatingRelevant);
	createParam(PICAM_CustomModulationSequenceRelString, asynParamInt32,
			&PICAM_CustomModulationSequenceRelevant);
	createParam(PICAM_DifEndingGateRelString, asynParamInt32,
			&PICAM_DifEndingGateRelevant);
	createParam(PICAM_DifStartingGateRelString, asynParamInt32,
			&PICAM_DifStartingGateRelevant);
	createParam(PICAM_EMIccdGainRelString, asynParamInt32,
			&PICAM_EMIccdGainRelevant);
	createParam(PICAM_EMIccdGainControlMode, asynParamInt32,
			&PICAM_EMIccdGainControlModeRelevant);
	createParam(PICAM_EnableIntensifierRelString, asynParamInt32,
			&PICAM_EnableIntensifierRelevant);
	createParam(PICAM_EnableModulationRelString, asynParamInt32,
			&PICAM_EnableModulationRelevant);
	createParam(PICAM_GatingModeRelString, asynParamInt32,
			&PICAM_GatingModeRelevant);
	createParam(PICAM_GatingSpeedRelString, asynParamInt32,
			&PICAM_GatingSpeedRelevant);
	createParam(PICAM_IntensifierDiameterRelString, asynParamInt32,
			&PICAM_IntensifierDiameterRelevant);
	createParam(PICAM_IntensifierGainRelString, asynParamInt32,
			&PICAM_IntensifierGainRelevant);
	createParam(PICAM_IntensifierOptionsRelString, asynParamInt32,
			&PICAM_IntensifierOptionsRelevant);
	createParam(PICAM_IntensifierStatusRelString, asynParamInt32,
			&PICAM_IntensifierStatusRelevant);
	createParam(PICAM_ModulationDurationRelString, asynParamInt32,
			&PICAM_ModulationDurationRelevant);
	createParam(PICAM_ModulationFrequencyRelString, asynParamInt32,
			&PICAM_ModulationFrequencyRelevant);
	createParam(PICAM_PhosphorDecayDelayRelString, asynParamInt32,
			&PICAM_PhosphorDecayDelayRelevant);
	createParam(PICAM_PhosphorDecayDelayResolutionRelString, asynParamInt32,
			&PICAM_PhosphorDecayDelayResolutionRelevant);
	createParam(PICAM_PhosphorTypeRelString, asynParamInt32,
			&PICAM_PhosphorTypeRelevant);
	createParam(PICAM_PhotocathodeSensitivityRelString, asynParamInt32,
			&PICAM_PhotocathodeSensitivityRelevant);
	createParam(PICAM_RepetitiveGateRelString, asynParamInt32,
			&PICAM_RepetitiveGateRelevant);
	createParam(PICAM_RepetitiveModulationPhaseRelString, asynParamInt32,
			&PICAM_RepetitiveModulationPhaseRelevant);
	createParam(PICAM_SequentialStartingModulationPhaseRelString,
			asynParamInt32, &PICAM_SequentialStartingModulationPhaseRelevant);
	createParam(PICAM_SequentialEndingModulationPhaseRelString, asynParamInt32,
			&PICAM_SequentialEndingModulationPhaseRelevant);
	createParam(PICAM_SequentialEndingGateRelString, asynParamInt32,
			&PICAM_SequentialEndingGateRelevant);
	createParam(PICAM_SequentialGateStepCountRelString, asynParamInt32,
			&PICAM_SequentialGateStepCountRelevant);
	createParam(PICAM_SequentialGateStepIterationsRelString, asynParamInt32,
			&PICAM_SequentialGateStepIterationsRelevant);
	createParam(PICAM_SequentialStartingGateRelString, asynParamInt32,
			&PICAM_SequentialStartingGateRelevant);
	createParam(PICAM_AdcAnalogGainRelString, asynParamInt32,
			&PICAM_AdcAnalogGainRelevant);
	createParam(PICAM_AdcBitDepthRelString, asynParamInt32,
			&PICAM_AdcBitDepthRelevant);
	createParam(PICAM_AdcEMGainRelString, asynParamInt32,
			&PICAM_AdcEMGainRelevant);
	createParam(PICAM_AdcQualityRelString, asynParamInt32,
			&PICAM_AdcQualityRelevant);
	createParam(PICAM_AdcSpeedRelString, asynParamInt32,
			&PICAM_AdcSpeedRelevant);
	createParam(PICAM_CorrectPixelBiasRelString, asynParamInt32,
			&PICAM_CorrectPixelBiasRelevant);
	createParam(PICAM_AuxOutputRelString, asynParamInt32,
			&PICAM_AuxOutputRelevant);
	createParam(PICAM_EnableModulationOutputSignalRelString, asynParamInt32,
			&PICAM_EnableModulationOutputSignalRelevant);
	createParam(PICAM_EnableModulationOutputSignalFrequencyRelString,
			asynParamInt32,
			&PICAM_EnableModulationOutputSignalFrequencyRelevant);
	createParam(PICAM_EnableModulationOutputSignalAmplitudeRelString,
			asynParamInt32,
			&PICAM_EnableModulationOutputSignalAmplitudeRelevant);
	createParam(PICAM_EnableSyncMasterRelString, asynParamInt32,
			&PICAM_EnableSyncMasterRelevant);
	createParam(PICAM_InvertOutputSignalRelString, asynParamInt32,
			&PICAM_InvertOutputSignalRelevant);
	createParam(PICAM_OutputSignalRelString, asynParamInt32,
			&PICAM_OutputSignalRelevant);
	createParam(PICAM_SyncMaster2DelayRelString, asynParamInt32,
			&PICAM_SyncMaster2DelayRelevant);
	createParam(PICAM_TriggerCouplingRelString, asynParamInt32,
			&PICAM_TriggerCouplingRelevant);
	createParam(PICAM_TriggerDeterminationRelString, asynParamInt32,
			&PICAM_TriggerDeterminationRelevant);
	createParam(PICAM_TriggerFrequencyRelString, asynParamInt32,
			&PICAM_TriggerFrequencyRelevant);
	createParam(PICAM_TriggerResponseRelString, asynParamInt32,
			&PICAM_TriggerResponseRelevant);
	createParam(PICAM_TriggerSourceRelString, asynParamInt32,
			&PICAM_TriggerSourceRelevant);
	createParam(PICAM_TriggerTerminationRelString, asynParamInt32,
			&PICAM_TriggerTerminationRelevant);
	createParam(PICAM_TriggerThresholdRelString, asynParamInt32,
			&PICAM_TriggerThresholdRelevant);
	createParam(PICAM_AccumulationsRelString, asynParamInt32,
			&PICAM_AccumulationsRelevant);
	createParam(PICAM_EnableNondestructiveReadoutRelString, asynParamInt32,
			&PICAM_EnableNondestructiveReadoutRelevant);
	createParam(PICAM_KineticsWindowHeightRelString, asynParamInt32,
			&PICAM_KineticsWindowHeightRelevant);
	createParam(PICAM_NondestructiveReadoutPeriodRelString, asynParamInt32,
			&PICAM_NondestructiveReadoutPeriodRelevant);
	createParam(PICAM_ReadoutControlModeRelString, asynParamInt32,
			&PICAM_ReadoutControlModeRelevant);
	createParam(PICAM_ReadoutOrientationRelString, asynParamInt32,
			&PICAM_ReadoutOrientationRelevant);
	createParam(PICAM_ReadoutPortCountRelString, asynParamInt32,
			&PICAM_ReadoutPortCountRelevant);
	createParam(PICAM_ReadoutTimeCalculationRelString, asynParamInt32,
			&PICAM_ReadoutTimeCalculationRelevant);
	createParam(PICAM_VerticalShiftRateRelString, asynParamInt32,
			&PICAM_VerticalShiftRateRelevant);
	createParam(PICAM_DisableDataFormattingRelString, asynParamInt32,
			&PICAM_DisableDataFormattingRelevant);
	createParam(PICAM_ExactReadoutCountMaximumRelString, asynParamInt32,
			&PICAM_ExactReadoutCountMaximumRelevant);
	createParam(PICAM_FrameRateCalculationRelString, asynParamInt32,
			&PICAM_FrameRateCalculationRelevant);
	createParam(PICAM_FrameSizeRelString, asynParamInt32,
			&PICAM_FrameSizeRelevant);
	createParam(PICAM_FramesPerReadoutRelString, asynParamInt32,
			&PICAM_FramesPerReadoutRelevant);
	createParam(PICAM_FrameStrideRelString, asynParamInt32,
			&PICAM_FrameStrideRelevant);
	createParam(PICAM_FrameTrackingBitDepthRelString, asynParamInt32,
			&PICAM_FrameTrackingBitDepthRelevant);
	createParam(PICAM_GateTrackingRelString, asynParamInt32,
			&PICAM_GateTrackingRelevant);
	createParam(PICAM_GateTrackingBitDepthRelString, asynParamInt32,
			&PICAM_GateTrackingBitDepthRelevant);
	createParam(PICAM_ModulationTrackingRelString, asynParamInt32,
			&PICAM_ModulationTrackingRelevant);
	createParam(PICAM_ModulationTrackingBitDepthRelString, asynParamInt32,
			&PICAM_ModulationTrackingBitDepthRelevant);
	createParam(PICAM_NormalizeOrientationRelString, asynParamInt32,
			&PICAM_NormalizeOrientationRelevant);
	createParam(PICAM_OnlineReadoutRateCalculationRelString, asynParamInt32,
			&PICAM_OnlineReadoutRateCalculationRelevant);
	createParam(PICAM_OrientationRelString, asynParamInt32,
			&PICAM_OrientationRelevant);
	createParam(PICAM_PhotonDetectionModeRelString, asynParamInt32,
			&PICAM_PhotonDetectionModeRelevant);
	createParam(PICAM_PhotonDetectionThresholdRelString, asynParamInt32,
			&PICAM_PhotonDetectionThresholdRelevant);
	createParam(PICAM_PixelBitDepthRelString, asynParamInt32,
			&PICAM_PixelBitDepthRelevant);
	createParam(PICAM_PixelFormatRelString, asynParamInt32,
			&PICAM_PixelFormatRelevant);
	createParam(PICAM_ReadoutCountRelString, asynParamInt32,
			&PICAM_ReadoutCountRelevant);
	createParam(PICAM_ReadoutRateCalculationRelString, asynParamInt32,
			&PICAM_ReadoutRateCalculationRelevant);
	createParam(PICAM_ReadoutStrideRelString, asynParamInt32,
			&PICAM_ReadoutStrideRelevant);
	createParam(PICAM_RoisRelString, asynParamInt32, &PICAM_RoisRelevant);
	createParam(PICAM_TimeStampBitDepthRelString, asynParamInt32,
			&PICAM_TimeStampBitDepthRelevant);
	createParam(PICAM_TimeStampResolutionRelString, asynParamInt32,
			&PICAM_TimeStampResolutionRelevant);
	createParam(PICAM_TimeStampsRelString, asynParamInt32,
			&PICAM_TimeStampsRelevant);
	createParam(PICAM_TrackFramesRelString, asynParamInt32,
			&PICAM_TrackFramesRelevant);
	createParam(PICAM_CcdCharacteristicsRelString, asynParamInt32,
			&PICAM_CcdCharacteristicsRelevant);
	createParam(PICAM_PixelGapHeightRelString, asynParamInt32,
			&PICAM_PixelGapHeightRelevant);
	createParam(PICAM_PixelGapWidthRelString, asynParamInt32,
			&PICAM_PixelGapWidthRelevant);
	createParam(PICAM_PixelHeightRelString, asynParamInt32,
			&PICAM_PixelHeightRelevant);
	createParam(PICAM_PixelWidthRelString, asynParamInt32,
			&PICAM_PixelWidthRelevant);
	createParam(PICAM_SensorActiveBottomMarginRelString, asynParamInt32,
			&PICAM_SensorActiveBottomMarginRelevant);
	createParam(PICAM_SensorActiveHeightRelString, asynParamInt32,
			&PICAM_SensorActiveHeightRelevant);
	createParam(PICAM_SensorActiveLeftMarginRelString, asynParamInt32,
			&PICAM_SensorActiveLeftMarginRelevant);
	createParam(PICAM_SensorActiveRightMarginRelString, asynParamInt32,
			&PICAM_SensorActiveRightMarginRelevant);
	createParam(PICAM_SensorActiveTopMarginRelString, asynParamInt32,
			&PICAM_SensorActiveTopMarginRelevant);
	createParam(PICAM_SensorActiveWidthRelString, asynParamInt32,
			&PICAM_SensorActiveWidthRelevant);
	createParam(PICAM_SensorMaskedBottomMarginRelString, asynParamInt32,
			&PICAM_SensorMaskedBottomMarginRelevant);
	createParam(PICAM_SensorMaskedHeightRelString, asynParamInt32,
			&PICAM_SensorMaskedHeightRelevant);
	createParam(PICAM_SensorMaskedTopMarginRelString, asynParamInt32,
			&PICAM_SensorMaskedTopMarginRelevant);
	createParam(PICAM_SensorSecondaryActiveHeightRelString, asynParamInt32,
			&PICAM_SensorSecondaryActiveHeightRelevant);
	createParam(PICAM_SensorSecondaryMaskedHeightRelString, asynParamInt32,
			&PICAM_SensorSecondaryMaskedHeightRelevant);
	createParam(PICAM_SensorTypeRelString, asynParamInt32,
			&PICAM_SensorTypeRelevant);
	createParam(PICAM_ActiveBottomMarginRelString, asynParamInt32,
			&PICAM_ActiveBottomMarginRelevant);
	createParam(PICAM_ActiveHeightRelString, asynParamInt32,
			&PICAM_ActiveHeightRelevant);
	createParam(PICAM_ActiveLeftMarginRelString, asynParamInt32,
			&PICAM_ActiveLeftMarginRelevant);
	createParam(PICAM_ActiveRightMarginRelString, asynParamInt32,
			&PICAM_ActiveRightMarginRelevant);
	createParam(PICAM_ActiveTopMarginRelString, asynParamInt32,
			&PICAM_ActiveTopMarginRelevant);
	createParam(PICAM_ActiveWidthRelString, asynParamInt32,
			&PICAM_ActiveWidthRelevant);
	createParam(PICAM_MaskedBottomMarginRelString, asynParamInt32,
			&PICAM_MaskedBottomMarginRelevant);
	createParam(PICAM_MaskedHeightRelString, asynParamInt32,
			&PICAM_MaskedHeightRelevant);
	createParam(PICAM_MaskedTopMarginRelString, asynParamInt32,
			&PICAM_MaskedTopMarginRelevant);
	createParam(PICAM_SecondaryActiveHeightRelString, asynParamInt32,
			&PICAM_SecondaryActiveHeightRelevant);
	createParam(PICAM_SecondaryMaskedHeightRelString, asynParamInt32,
			&PICAM_SecondaryMaskedHeightRelevant);
	createParam(PICAM_CleanBeforeExposureRelString, asynParamInt32,
			&PICAM_CleanBeforeExposureRelevant);
	createParam(PICAM_CleanCycleCountRelString, asynParamInt32,
			&PICAM_CleanCycleCountRelevant);
	createParam(PICAM_CleanCycleHeightRelString, asynParamInt32,
			&PICAM_CleanCycleHeightRelevant);
	createParam(PICAM_CleanSectionFinalHeightRelString, asynParamInt32,
			&PICAM_CleanSectionFinalHeightRelevant);
	createParam(PICAM_CleanSectionFinalHeightCountRelString, asynParamInt32,
			&PICAM_CleanSectionFinalHeightCountRelevant);
	createParam(PICAM_CleanSerialRegisterRelString, asynParamInt32,
			&PICAM_CleanSerialRegisterRelevant);
	createParam(PICAM_CleanUntilTriggerRelString, asynParamInt32,
			&PICAM_CleanUntilTriggerRelevant);
	createParam(PICAM_DisableCoolingFanRelString, asynParamInt32,
			&PICAM_DisableCoolingFanRelevant);
	createParam(PICAM_EnableSensorWindowHeaterRelString, asynParamInt32,
			&PICAM_EnableSensorWindowHeaterRelevant);
	createParam(PICAM_SensorTemperatureReadingRelString, asynParamInt32,
			&PICAM_SensorTemperatureReadingRelevant);
	createParam(PICAM_SensorTemperatureSetPointRelString, asynParamInt32,
			&PICAM_SensorTemperatureSetPointRelevant);
	createParam(PICAM_SensorTemperatureStatusRelString, asynParamInt32,
			&PICAM_SensorTemperatureStatusRelevant);

	status = setStringParam(ADManufacturer, "Princeton Instruments");
	status |= setStringParam(ADModel, "Not Connected");
	status |= setIntegerParam(NDArraySize, 0);
	status |= setIntegerParam(NDDataType, NDUInt16);

	status |= setStringParam(PICAM_VersionNumber, "NOT DETERMINED1");
	status |= setIntegerParam(PICAM_AvailableCameras, 0);
	status |= setStringParam(PICAM_CameraInterface, "NOT DETERMINED2");
	status |= setStringParam(PICAM_SensorName, "NOT DETERMINED3");
	status |= setStringParam(PICAM_SerialNumber, "NOT DETERMINED4");
	status |= setStringParam(PICAM_FirmwareRevision, "NOT DETERMINED5");
	status |= setIntegerParam(PICAM_UnavailableCameras, 0);
	status |= setStringParam(PICAM_CameraInterfaceUnavailable,
			"NOT DETERMINED2");
	status |= setStringParam(PICAM_SensorNameUnavailable, "NOT DETERMINED3");
	status |= setStringParam(PICAM_SerialNumberUnavailable, "NOT DETERMINED4");
	status |= setStringParam(PICAM_FirmwareRevisionUnavailable,
			"NOT DETERMINED5");
	callParamCallbacks();
	unlock();

	piLoadAvailableCameraIDs();
	setIntegerParam(PICAM_AvailableCameras, 0);
	callParamCallbacks();

	// Comment out for now.  This is making the IOC crash.  Need to move on to
	// reading parameters.
	piLoadUnavailableCameraIDs();
	if (status) {
		asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
				"%s:%s: unable to set camera parameters\n", driverName,
				functionName);
		return;
	}
	initializeDetector();

}

ADPICam::~ADPICam() {
	Picam_UninitializeLibrary();
}

/**
 Initialize Picam property initialization
 */
asynStatus ADPICam::initializeDetector() {
	piint versionMajor, versionMinor, versionDistribution, versionReleased;

	const char *errorString;
	const char *triggerString;
	static const char* functionName = "initializeDetector";
	bool retVal = true;
	char picamVersion[16];
	int status = asynSuccess;
	PicamError error;

	Picam_GetVersion(&versionMajor, &versionMinor, &versionDistribution,
			&versionReleased);
	asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
			"%s:%s Initialized PICam Version %d.%d.%d.%d\n", driverName,
			functionName, versionMajor, versionMinor, versionDistribution,
			versionReleased);
	sprintf(picamVersion, "%d.%d.%d.%d", versionMajor, versionMinor,
			versionDistribution, versionReleased);
	status |= setStringParam(PICAM_VersionNumber, (const char *) picamVersion);
	if (status) {
		asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
				"%s:%s: unable to set camera parameters\n", driverName,
				functionName);
		return asynError;
	}
	error = PicamAdvanced_RegisterForDiscovery(piCameraDiscovered);
	if (error != PicamError_None) {

		asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
				"%s:%s Fail to Register for Camera Discovery :%s\n", driverName,
				functionName, errorString);
		return asynError;
	}
	error = PicamAdvanced_DiscoverCameras();
	if (error != PicamError_None) {

		asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
				"%s:%s Fail to start camera discovery :%s\n", driverName,
				functionName, errorString);
		return asynError;
	}

	return (asynStatus) status;
}

asynStatus ADPICam::readEnum(asynUser *pasynUser, char *strings[], int values[],
		int severities[], size_t nElements, size_t *nIn) {
	char enumString[64];
	int status = asynSuccess;
	static const char *functionName = "readEnum";
	const char *modelString;
	const char *errorString;
	PicamError error;
	int function = pasynUser->reason;

	asynPrint(pasynUserSelf, ASYN_TRACE_FLOW, "*******%s:%s: entry\n",
			driverName, functionName);

	*nIn = 0;
	if (function == PICAM_AvailableCameras) {
		asynPrint(pasynUserSelf, ASYN_TRACE_FLOW, "Getting Available IDs\n");
		asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
				"%s:%s availableCamerasCount %d\n", driverName, functionName,
				availableCamerasCount);
		for (int ii = 0; ii < availableCamerasCount; ii++) {
			Picam_GetEnumerationString(PicamEnumeratedType_Model,
					(piint) availableCameraIDs[ii].model, &modelString);
			pibln camConnected = false;
			Picam_IsCameraIDConnected(availableCameraIDs, &camConnected);
			sprintf(enumString, "%s", modelString);
			asynPrint(pasynUser, ASYN_TRACE_FLOW,
					"\n%s:%s: \nCamera[%d]\n---%s\n---%d\n---%s\n---%s\n",
					driverName, functionName, ii, modelString,
					availableCameraIDs[ii].computer_interface,
					availableCameraIDs[ii].sensor_name,
					availableCameraIDs[ii].serial_number);
			if (strings[*nIn])
				free(strings[*nIn]);

			Picam_DestroyString(modelString);
			strings[*nIn] = epicsStrDup(enumString);
			values[*nIn] = ii;
			severities[*nIn] = 0;
			(*nIn)++;
		}

	} else if (function == PICAM_UnavailableCameras) {
		asynPrint(pasynUserSelf, ASYN_TRACE_FLOW, "Getting Unavailable IDs\n");
		for (int ii = 0; ii < unavailableCamerasCount; ii++) {
			Picam_GetEnumerationString(PicamEnumeratedType_Model,
					(piint) unavailableCameraIDs[ii].model, &modelString);
			sprintf(enumString, "%s", modelString);
			asynPrint(pasynUser, ASYN_TRACE_ERROR,
					"\n%s:%s: \nCamera[%d]\n---%s\n---%d\n---%s\n---%s\n",
					driverName, functionName, ii, modelString,
					unavailableCameraIDs[ii].computer_interface,
					unavailableCameraIDs[ii].sensor_name,
					unavailableCameraIDs[ii].serial_number);
			if (strings[*nIn])
				free(strings[*nIn]);

			Picam_DestroyString(modelString);
			strings[*nIn] = epicsStrDup(enumString);
			values[*nIn] = ii;
			severities[*nIn] = 0;
			(*nIn)++;
		}
	} else if (function == ADTriggerMode) {
		const char *triggerResponseString;
		const PicamCollectionConstraint *constraints;
		if (currentCameraHandle != NULL) {
			Picam_GetParameterCollectionConstraint(currentCameraHandle,
					PicamParameter_TriggerResponse,
					PicamConstraintCategory_Capable, &constraints);
			for (int ii = 0; ii < constraints->values_count; ii++) {
				Picam_GetEnumerationString(PicamEnumeratedType_TriggerResponse,
						(int) constraints->values_array[ii],
						&triggerResponseString);
				if (strings[*nIn])
					free(strings[*nIn]);
				strings[*nIn] = epicsStrDup(triggerResponseString);
				values[*nIn] = (int) constraints->values_array[ii];
				severities[*nIn] = 0;
				(*nIn)++;
				Picam_DestroyString(triggerResponseString);
			}
		}
	} else if (function == PICAM_AdcAnalogGain) {
		const PicamCollectionConstraint *constraints;
		const char *adcAnalogGainString;
		if (currentCameraHandle != NULL) {
			Picam_GetParameterCollectionConstraint(currentCameraHandle,
					PicamParameter_AdcAnalogGain,
					PicamConstraintCategory_Capable, &constraints);
			for (int ii = 0; ii < constraints->values_count; ii++) {
				Picam_GetEnumerationString(PicamEnumeratedType_AdcAnalogGain,
						(int) constraints->values_array[ii],
						&adcAnalogGainString);
				if (strings[*nIn])
					free(strings[*nIn]);
				strings[*nIn] = epicsStrDup(adcAnalogGainString);
				values[*nIn] = (int) constraints->values_array[ii];
				severities[*nIn] = 0;
				(*nIn)++;
				Picam_DestroyString(adcAnalogGainString);
			}
		}
	} else if (function == PICAM_AdcQuality) {
		const PicamCollectionConstraint *constraints;
		const char *adcQualityString;
		if (currentCameraHandle != NULL) {
			Picam_GetParameterCollectionConstraint(currentCameraHandle,
					PicamParameter_AdcQuality,
					PicamConstraintCategory_Capable, &constraints);
			for (int ii = 0; ii < constraints->values_count; ii++) {
				Picam_GetEnumerationString(PicamEnumeratedType_AdcQuality,
						(int) constraints->values_array[ii],
						&adcQualityString);
				if (strings[*nIn])
					free(strings[*nIn]);
				strings[*nIn] = epicsStrDup(adcQualityString);
				values[*nIn] = (int) constraints->values_array[ii];
				severities[*nIn] = 0;
				(*nIn)++;
				Picam_DestroyString(adcQualityString);
			}
		}
	} else if (function == PICAM_AdcBitDepth) {
		const PicamCollectionConstraint *constraints;
		char adcBitDepthString[12];
		if (currentCameraHandle != NULL) {
			Picam_GetParameterCollectionConstraint(currentCameraHandle,
					PicamParameter_AdcBitDepth,
					PicamConstraintCategory_Capable, &constraints);
			for (int ii = 0; ii < constraints->values_count; ii++) {
				sprintf(adcBitDepthString, "%d", (int)constraints->values_array[ii]);
				if (strings[*nIn])
					free(strings[*nIn]);
				strings[*nIn] = epicsStrDup(adcBitDepthString);
				values[*nIn] = (int) constraints->values_array[ii];
				severities[*nIn] = 0;
				(*nIn)++;
			}
		}
	} else if (function == PICAM_AdcSpeed) {
		const PicamCollectionConstraint *constraints;
		char adcSpeedString[12];
		if (currentCameraHandle != NULL) {
			Picam_GetParameterCollectionConstraint(currentCameraHandle,
					PicamParameter_AdcSpeed,
					PicamConstraintCategory_Capable, &constraints);
			for (int ii = 0; ii < constraints->values_count; ii++) {
				sprintf(adcSpeedString, "%f",constraints->values_array[ii] );
				if (strings[*nIn])
					free(strings[*nIn]);
				strings[*nIn] = epicsStrDup(adcSpeedString);
				values[*nIn] = ii;
				severities[*nIn] = 0;
				(*nIn)++;
			}
		}
	} else {
		return ADDriver::readEnum(pasynUser, strings, values, severities,
				nElements, nIn);
	}
	if (status) {
		asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
				"%s:%s: error calling enum functions, status=%d\n", driverName,
				functionName, status);
	}
	asynPrint(pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit\n", driverName,
			functionName);
	return asynSuccess;
}

void ADPICam::report(FILE *fp, int details) {
	pibln picamInitialized;
	static const char *functionName = "report";
	const char *modelString;
	char enumString[64];
	PicamError error = PicamError_None;
	const PicamRoisConstraint *roisConstraints;
	const char *errorString;

	fprintf(fp, "############ %s:%s ###############\n", driverName,
			functionName);
	Picam_IsLibraryInitialized(&picamInitialized);
	if (!picamInitialized) {
		asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
				"%s:%s: Error: Picam is not initialized!\n", driverName,
				functionName);
		return;

	}
	char versionNumber[40];
	getStringParam(PICAM_VersionNumber, 40, versionNumber);
	fprintf(fp, "%s:%s Initialized PICam Version %s\n", driverName,
			functionName, versionNumber);
	fprintf(fp, "----------------\n");
	fprintf(fp, "%s:%s availableCamerasCount %d\n", driverName, functionName,
			availableCamerasCount);
	for (int ii = 0; ii < availableCamerasCount; ii++) {
		fprintf(fp, "Available Camera[%d]\n", ii);
		Picam_GetEnumerationString(PicamEnumeratedType_Model,
				(piint) availableCameraIDs[ii].model, &modelString);
		sprintf(enumString, "%s", modelString);
		fprintf(fp, "\n---%s\n---%d\n---%s\n---%s\n", modelString,
				availableCameraIDs[ii].computer_interface,
				availableCameraIDs[ii].sensor_name,
				availableCameraIDs[ii].serial_number);
	}
	fprintf(fp, "----------------\n");
	fprintf(fp, "%s:%s unavailableCamerasCount %d\n", driverName, functionName,
			unavailableCamerasCount);
	for (int ii = 0; ii < unavailableCamerasCount; ii++) {
		fprintf(fp, "Unavailable Camera[%d]\n", ii);
		Picam_GetEnumerationString(PicamEnumeratedType_Model,
				(piint) unavailableCameraIDs[ii].model, &modelString);
		sprintf(enumString, "%s", modelString);
		fprintf(fp, "\n---%s\n---%d\n---%s\n---%s\n", modelString,
				unavailableCameraIDs[ii].computer_interface,
				unavailableCameraIDs[ii].sensor_name,
				unavailableCameraIDs[ii].serial_number);
	}
	fprintf(fp, "----------------\n");

	error = Picam_GetParameterRoisConstraint(currentCameraHandle,
			PicamParameter_Rois, PicamConstraintCategory_Required,
			&roisConstraints);
	if (error != PicamError_None) {
		Picam_GetEnumerationString(PicamEnumeratedType_Error, error,
				&errorString);
		asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
				"%s:%s Error retrieving rois constraints %s\n", driverName,
				functionName, errorString);
		Picam_DestroyString(errorString);
	}
	fprintf(fp, "ROI Constraints\n"
			"--X min %d, X max %d\n"
			"--Y min %d, Y max %d\n"
			"--width min %d,  width max %d\n"
			"--height %d, height max %d\n"
			"--Rules 0x%x\n",
//			"--X bin min %d, X bin max %d\n"
//			"--Y bin min %d, Y bin max %d\n",
			roisConstraints->x_constraint.minimum,
			roisConstraints->x_constraint.maximum,
			roisConstraints->y_constraint.minimum,
			roisConstraints->y_constraint.maximum,
			roisConstraints->width_constraint.minimum,
			roisConstraints->width_constraint.maximum,
			roisConstraints->height_constraint.minimum,
			roisConstraints->height_constraint.maximum,
			roisConstraints->rules);
//			roisConstraints->x_.minimum,
//			roisConstraints->x_constraint.minimum);

	if (details > 20){
		piint demoModelCount;
		PicamCameraID demoID;
		const PicamModel * demoModels;

		error = Picam_GetAvailableDemoCameraModels(&demoModels, &demoModelCount);
		for (int ii=0; ii<demoModelCount; ii++){
			const char* modelString;
			error = Picam_GetEnumerationString(PicamEnumeratedType_Model,
					demoModels[ii],
					&modelString);
			fprintf(fp, "demoModel[%d]: %s\n", ii, modelString);
			Picam_DestroyString(modelString);
		}
		Picam_DestroyModels(demoModels);

	}
}

asynStatus ADPICam::writeInt32(asynUser *pasynUser, epicsInt32 value) {
	int function = pasynUser->reason;
	int status = asynSuccess;
	static const char *functionName = "writeInt32";
	const PicamRois * paramRois;
	PicamError error = PicamError_None;
	const PicamRoisConstraint *roisConstraint;
	const char* errorString;
	int sizeX, sizeY, binX, binY, minX, minY;

	if (function == PICAM_AvailableCameras) {
		piSetSelectedCamera(pasynUser, (int) value);
		piSetParameterValuesFromSelectedCamera();
	} else if (function == PICAM_UnavailableCameras) {
		piSetSelectedUnavailableCamera(pasynUser, (int) value);
	} else if (function == PICAM_AdcAnalogGain) {
		Picam_SetParameterIntegerValue(currentCameraHandle,
				PicamParameter_AdcAnalogGain,
				value);
	} else if (function == PICAM_AdcBitDepth) {
		Picam_SetParameterIntegerValue(currentCameraHandle,
				PicamParameter_AdcBitDepth,
				value);
	} else if (function == PICAM_AdcQuality) {
		Picam_SetParameterIntegerValue(currentCameraHandle,
				PicamParameter_AdcQuality,
				value);
	} else if (function == PICAM_AdcSpeed) {
		piflt fvalue;
		const PicamCollectionConstraint* speedCollection;
		Picam_GetParameterCollectionConstraint(currentCameraHandle,
				PicamParameter_AdcSpeed,
				PicamConstraintCategory_Capable,
				&speedCollection);
		Picam_SetParameterFloatingPointValue(currentCameraHandle,
				PicamParameter_AdcSpeed,
				speedCollection->values_array[value]);
	} else if (function == ADSizeX) {
		int xstatus = asynSuccess;
		sizeX = value;
		xstatus |= getIntegerParam(ADSizeY, &sizeY);
		xstatus |= getIntegerParam(ADBinX, &binX);
		xstatus |= getIntegerParam(ADBinY, &binY);
		xstatus |= getIntegerParam(ADMinX, &minX);
		xstatus |= getIntegerParam(ADMinY, &minY);
		xstatus |= piSetRois(minX, minY, sizeX, sizeY, binX, binY);
	} else if (function == ADSizeY) {
		int xstatus = asynSuccess;
		xstatus |= getIntegerParam(ADSizeX, &sizeX);
		sizeY = value;
		xstatus |= getIntegerParam(ADBinX, &binX);
		xstatus |= getIntegerParam(ADBinY, &binY);
		xstatus |= getIntegerParam(ADMinX, &minX);
		xstatus |= getIntegerParam(ADMinY, &minY);
		xstatus |= piSetRois(minX, minY, sizeX, sizeY, binX, binY);
	} else if (function == ADBinX) {
		int xstatus = asynSuccess;
		xstatus |= getIntegerParam(ADSizeX, &sizeX);
		xstatus |= getIntegerParam(ADSizeY, &sizeY);
		binX = value;
		xstatus |= getIntegerParam(ADBinY, &binY);
		xstatus |= getIntegerParam(ADMinX, &minX);
		xstatus |= getIntegerParam(ADMinY, &minY);
		xstatus |= piSetRois(minX, minY, sizeX, sizeY, binX, binY);
	} else if (function == ADBinY) {
		int xstatus = asynSuccess;
		xstatus |= getIntegerParam(ADSizeX, &sizeX);
		xstatus |= getIntegerParam(ADSizeY, &sizeY);
		xstatus |= getIntegerParam(ADBinX, &binX);
		binY = value;
		xstatus |= getIntegerParam(ADMinX, &minX);
		xstatus |= getIntegerParam(ADMinY, &minY);
		xstatus |= piSetRois(minX, minY, sizeX, sizeY, binX, binY);
	} else if (function == ADMinX) {
		int xstatus = asynSuccess;
		xstatus |= getIntegerParam(ADSizeX, &sizeX);
		xstatus |= getIntegerParam(ADSizeY, &sizeY);
		xstatus |= getIntegerParam(ADBinX, &binX);
		xstatus |= getIntegerParam(ADBinY, &binY);
		minX = value;
		xstatus |= getIntegerParam(ADMinY, &minY);
		xstatus |= piSetRois(minX, minY, sizeX, sizeY, binX, binY);
	} else if (function == ADMinY) {
		int xstatus = asynSuccess;
		xstatus |= getIntegerParam(ADSizeX, &sizeX);
		xstatus |= getIntegerParam(ADSizeY, &sizeY);
		xstatus |= getIntegerParam(ADBinX, &binX);
		xstatus |= getIntegerParam(ADBinY, &binY);
		xstatus |= getIntegerParam(ADMinX, &minX);
		minY = value;
		xstatus |= piSetRois(minX, minY, sizeX, sizeY, binX, binY);
	} else {
		/* If this parameter belongs to a base class call its method */
		if (function < PICAM_FIRST_PARAM) {
			status = ADDriver::writeInt32(pasynUser, value);
		}
	}

	/* Do callbacks so higher layers see any changes */
	callParamCallbacks();

	if (status)
		asynPrint(pasynUser, ASYN_TRACE_ERROR,
				"%s:%s: error, status=%d function=%d, value=%d\n", driverName,
				functionName, status, function, value);
	else
		asynPrint(pasynUser, ASYN_TRACEIO_DRIVER,
				"%s:%s: function=%d, value=%d\n", driverName, functionName,
				function, value);

	return (asynStatus) status;

}

asynStatus ADPICam::writeFloat64(asynUser *pasynUser, epicsFloat64 value) {
	int function = pasynUser->reason;
	int status = asynSuccess;
	static const char *functionName = "writeFloat64";
	const char *errorString;
	PicamError error = PicamError_None;

	asynPrint(pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entry\n", driverName,
			functionName);

	if (function == ADTemperature) {
		error = Picam_SetParameterFloatingPointValue(currentCameraHandle,
				PicamParameter_SensorTemperatureSetPoint, (piflt) value);
		if (error != PicamError_None) {
			Picam_GetEnumerationString(PicamEnumeratedType_Error, error,
					&errorString);

			asynPrint(pasynUser, ASYN_TRACE_ERROR,
					"%s:%s error writing SensorTemperatureSetPoint to %f\n"
							"Reason %s\n", driverName, functionName, value,
					errorString);
			Picam_DestroyString(errorString);
			return asynError;
		}
	}
	if (function == ADAcquireTime) {
		error = Picam_SetParameterFloatingPointValue(currentCameraHandle,
				PicamParameter_ExposureTime, (piflt) value);
	} else {
		/* If this parameter belongs to a base class call its method */
		if (function < PICAM_FIRST_PARAM) {
			status = ADDriver::writeFloat64(pasynUser, value);
		}

	}
	/* Do callbacks so higher layers see any changes */
	callParamCallbacks();

	if (status)
		asynPrint(pasynUser, ASYN_TRACE_ERROR,
				"%s:%s: error, status=%d function=%d, value=%f\n", driverName,
				functionName, status, function, value);
	else
		asynPrint(pasynUser, ASYN_TRACEIO_DRIVER,
				"%s:%s: function=%d, value=%f\n", driverName, functionName,
				function, value);
	return (asynStatus) status;
	asynPrint(pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit\n", driverName,
			functionName);

}

asynStatus ADPICam::piAddDemoCamera(const char *demoCameraName){
	const char * functionName = "piAddDemoCamera";
	int status = asynSuccess;
	const PicamModel *demoModels;
	piint demoModelCount;
	bool modelFoundInList = false;
	PicamCameraID demoID;
	PicamError error = PicamError_None;
	const char *errorString;

	pibln libInitialized = true;
	Picam_IsLibraryInitialized(&libInitialized);

	if (libInitialized){
		error = Picam_GetAvailableDemoCameraModels(&demoModels, &demoModelCount);
		for (int ii=0; ii<demoModelCount; ii++){
			const char* modelString;
			error = Picam_GetEnumerationString(PicamEnumeratedType_Model,
					demoModels[ii],
					&modelString);
			if (strcmp(demoCameraName, modelString) == 0 ){
				modelFoundInList = true;
				error = Picam_ConnectDemoCamera(demoModels[ii], "ADDemo", &demoID);
				if (error == PicamError_None) {
					printf("%s:%s Adding camera demoCamera[%d] %s\n",
							driverName,
							functionName,
							demoModels[ii],
							demoCameraName);
					//ADPICam_Instance->piUpdateAvailableCamerasList();
				}
				else {
					Picam_GetEnumerationString(PicamEnumeratedType_Error, error, &errorString);
					printf("%s:%s Error Adding camera demoCamera[%d] %s, %s\n",
							driverName,
							functionName,
							ii,
							demoCameraName,
							errorString);
				}
				ii = demoModelCount;
			}
			Picam_DestroyString(modelString);
		}
		Picam_DestroyModels(demoModels);
	}
	else {
		status = asynError;
	}


	return (asynStatus)status;
}

PicamError PIL_CALL ADPICam::piCameraDiscovered(const PicamCameraID *id,
		PicamHandle device, PicamDiscoveryAction action) {
	int status;
	status = ADPICam_Instance->piHandleCameraDiscovery(id, device, action);

	return PicamError_None;
}

/**
 Set all PICAM parameter relevance parameters to false
 */
asynStatus ADPICam::piClearParameterRelevance(asynUser *pasynUser) {
	int iParam;
	int status = asynSuccess;

	for (iParam = PICAM_ExposureTimeRelevant;
			iParam <= PICAM_SensorTemperatureStatusRelevant; iParam++) {
		status |= setIntegerParam(iParam, 0);
	}
	return (asynStatus) status;
}

/**
 *
 */
asynStatus ADPICam::piLoadAvailableCameraIDs() {
	PicamError error;
	const char *errorString;
	const char *functionName = "piLoadAvailableCameraIDs";
	int status = asynSuccess;

	if (availableCamerasCount != 0) {
		error = Picam_DestroyCameraIDs(availableCameraIDs);
		availableCamerasCount = 0;
		if (error != PicamError_None) {
			Picam_GetEnumerationString(PicamEnumeratedType_Error, (piint) error,
					&errorString);
			asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
					"%s:%s: Picam_DestroyCameraIDs error %s", driverName,
					functionName, errorString);
			Picam_DestroyString(errorString);
			return asynError;
		}
	}
	error = Picam_GetAvailableCameraIDs(&availableCameraIDs,
			&availableCamerasCount);
	if (error != PicamError_None) {
		Picam_GetEnumerationString(PicamEnumeratedType_Error, (piint) error,
				&errorString);
		asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
				"%s%s: Picam_GetAvailableCameraIDs error %s", driverName,
				functionName, errorString);
		Picam_DestroyString(errorString);
		return asynError;
	}
	return (asynStatus) status;
}

asynStatus ADPICam::piLoadUnavailableCameraIDs() {
	PicamError error;
	const char *errorString;
	const char *functionName = "piLoadUnavailableCameraIDs";
	int status = asynSuccess;

	if (unavailableCamerasCount != 0) {
		error = Picam_DestroyCameraIDs(unavailableCameraIDs);
		unavailableCamerasCount = 0;
		if (error != PicamError_None) {
			Picam_GetEnumerationString(PicamEnumeratedType_Error, (piint) error,
					&errorString);
			asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
					"%s:%s: Picam_DestroyCameraIDs error %s", driverName,
					functionName, errorString);
			Picam_DestroyString(errorString);
			return asynError;
		}
	}
	error = Picam_GetUnavailableCameraIDs(&unavailableCameraIDs,
			&unavailableCamerasCount);
	if (error != PicamError_None) {
		Picam_GetEnumerationString(PicamEnumeratedType_Error, (piint) error,
				&errorString);
		asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
				"%s%s: Picam_GetUnavailableCameraIDs error %s", driverName,
				functionName, errorString);
		Picam_DestroyString(errorString);
		return asynError;
	}
	return (asynStatus) status;
}

/**
 *
 */
int ADPICam::piLookupDriverParameter(PicamParameter parameter) {
	int driverParameter = -1;
	const char *paramString;
	const char *functionName = "piLookupDriverParameter";

	switch (parameter) {
	case PicamParameter_Accumulations:
		break;
	case PicamParameter_ActiveBottomMargin:
		break;
	case PicamParameter_ActiveHeight:
		break;
	case PicamParameter_ActiveLeftMargin:
		break;
	case PicamParameter_ActiveRightMargin:
		break;
	case PicamParameter_ActiveTopMargin:
		break;
	case PicamParameter_ActiveWidth:
		break;
	case PicamParameter_AdcAnalogGain:
		driverParameter = PICAM_AdcAnalogGain;
		break;
	case PicamParameter_AdcBitDepth:
		driverParameter = PICAM_AdcBitDepth;
		break;
	case PicamParameter_AdcEMGain:
		break;
	case PicamParameter_AdcQuality:
		driverParameter = PICAM_AdcQuality;
		break;
	case PicamParameter_AdcSpeed:
		driverParameter = PICAM_AdcSpeed;
		break;
	case PicamParameter_AuxOutput:
		break;
	case PicamParameter_BracketGating:
		break;
	case PicamParameter_CcdCharacteristics:
		break;
	case PicamParameter_CleanBeforeExposure:
		break;
	case PicamParameter_CleanCycleCount:
		break;
	case PicamParameter_CleanCycleHeight:
		break;
	case PicamParameter_CleanSectionFinalHeight:
		break;
	case PicamParameter_CleanSectionFinalHeightCount:
		break;
	case PicamParameter_CleanSerialRegister:
		break;
	case PicamParameter_CleanUntilTrigger:
		break;
	case PicamParameter_CorrectPixelBias:
		break;
	case PicamParameter_CustomModulationSequence:
		break;
	case PicamParameter_DifEndingGate:
		break;
	case PicamParameter_DifStartingGate:
		break;
	case PicamParameter_DisableCoolingFan:
		break;
	case PicamParameter_DisableDataFormatting:
		break;
	case PicamParameter_EMIccdGain:
		break;
	case PicamParameter_EMIccdGainControlMode:
		break;
	case PicamParameter_EnableIntensifier:
		break;
	case PicamParameter_EnableModulation:
		break;
	case PicamParameter_EnableModulationOutputSignal:
		break;
	case PicamParameter_EnableNondestructiveReadout:
		break;
	case PicamParameter_EnableSensorWindowHeater:
		break;
	case PicamParameter_EnableSyncMaster:
		break;
	case PicamParameter_ExactReadoutCountMaximum:
		break;
	case PicamParameter_ExposureTime:
		driverParameter = ADAcquireTime;
		break;
	case PicamParameter_FrameRateCalculation:
		driverParameter = PICAM_FrameRateCalc;
		break;
	case PicamParameter_FrameSize:
		driverParameter = NDArraySize;
		break;
	case PicamParameter_FramesPerReadout:
		driverParameter = ADNumExposures;
		break;
	case PicamParameter_FrameStride:
		break;
	case PicamParameter_FrameTrackingBitDepth:
		break;
	case PicamParameter_GateTracking:
		break;
	case PicamParameter_GateTrackingBitDepth:
		break;
	case PicamParameter_GatingMode:
		break;
	case PicamParameter_GatingSpeed:
		break;
	case PicamParameter_IntensifierDiameter:
		break;
	case PicamParameter_IntensifierGain:
		break;
	case PicamParameter_IntensifierOptions:
		break;
	case PicamParameter_IntensifierStatus:
		break;
	case PicamParameter_InvertOutputSignal:
		break;
	case PicamParameter_KineticsWindowHeight:
		break;
	case PicamParameter_MaskedBottomMargin:
		break;
	case PicamParameter_MaskedHeight:
		break;
	case PicamParameter_MaskedTopMargin:
		break;
	case PicamParameter_ModulationDuration:
		break;
	case PicamParameter_ModulationFrequency:
		break;
	case PicamParameter_ModulationOutputSignalAmplitude:
		break;
	case PicamParameter_ModulationOutputSignalFrequency:
		break;
	case PicamParameter_ModulationTracking:
		break;
	case PicamParameter_ModulationTrackingBitDepth:
		break;
	case PicamParameter_NondestructiveReadoutPeriod:
		break;
	case PicamParameter_NormalizeOrientation:
		break;
	case PicamParameter_OnlineReadoutRateCalculation:
		driverParameter = PICAM_OnlineReadoutRateCalc;
		break;
	case PicamParameter_Orientation:
		break;
	case PicamParameter_OutputSignal:
		break;
	case PicamParameter_PhosphorDecayDelay:
		break;
	case PicamParameter_PhosphorDecayDelayResolution:
		break;
	case PicamParameter_PhosphorType:
		break;
	case PicamParameter_PhotocathodeSensitivity:
		break;
	case PicamParameter_PhotonDetectionMode:
		break;
	case PicamParameter_PhotonDetectionThreshold:
		break;
	case PicamParameter_PixelBitDepth:
		break;
	case PicamParameter_PixelFormat:
		break;
	case PicamParameter_PixelGapHeight:
		break;
	case PicamParameter_PixelGapWidth:
		break;
	case PicamParameter_PixelHeight:
		break;
	case PicamParameter_PixelWidth:
		break;
	case PicamParameter_ReadoutControlMode:
		break;
	case PicamParameter_ReadoutCount:
		driverParameter = ADNumImages;
		break;
	case PicamParameter_ReadoutOrientation:
		break;
	case PicamParameter_ReadoutPortCount:
		break;
	case PicamParameter_ReadoutRateCalculation:
		driverParameter = PICAM_ReadoutRateCalc;
		break;
	case PicamParameter_ReadoutStride:
		break;
	case PicamParameter_ReadoutTimeCalculation:
		driverParameter = PICAM_ReadoutTimeCalc;
		break;
	case PicamParameter_RepetitiveGate:
		break;
	case PicamParameter_RepetitiveModulationPhase:
		break;
	case PicamParameter_Rois:
		break;
	case PicamParameter_SecondaryActiveHeight:
		break;
	case PicamParameter_SecondaryMaskedHeight:
		break;
	case PicamParameter_SensorActiveBottomMargin:
		break;
	case PicamParameter_SensorActiveHeight:
		driverParameter = ADMaxSizeY;
		break;
	case PicamParameter_SensorActiveLeftMargin:
		break;
	case PicamParameter_SensorActiveRightMargin:
		break;
	case PicamParameter_SensorActiveTopMargin:
		break;
	case PicamParameter_SensorActiveWidth:
		driverParameter = ADMaxSizeX;
		break;
	case PicamParameter_SensorMaskedBottomMargin:
		break;
	case PicamParameter_SensorMaskedHeight:
		break;
	case PicamParameter_SensorMaskedTopMargin:
		break;
	case PicamParameter_SensorSecondaryActiveHeight:
		break;
	case PicamParameter_SensorSecondaryMaskedHeight:
		break;
	case PicamParameter_SensorTemperatureReading:
		driverParameter = ADTemperatureActual;
		break;
	case PicamParameter_SensorTemperatureSetPoint:
		driverParameter = ADTemperature;
		break;
	case PicamParameter_SensorTemperatureStatus:
		break;
	case PicamParameter_SensorType:
		break;
	case PicamParameter_SequentialEndingGate:
		break;
	case PicamParameter_SequentialEndingModulationPhase:
		break;
	case PicamParameter_SequentialGateStepCount:
		break;
	case PicamParameter_SequentialGateStepIterations:
		break;
	case PicamParameter_SequentialStartingGate:
		break;
	case PicamParameter_SequentialStartingModulationPhase:
		break;
	case PicamParameter_ShutterClosingDelay:
		driverParameter = ADShutterCloseDelay;
		break;
	case PicamParameter_ShutterDelayResolution:
		break;
	case PicamParameter_ShutterOpeningDelay:
		driverParameter = ADShutterOpenDelay;
		break;
	case PicamParameter_ShutterTimingMode:
		break;
	case PicamParameter_SyncMaster2Delay:
		break;
	case PicamParameter_TimeStampBitDepth:
		break;
	case PicamParameter_TimeStampResolution:
		break;
	case PicamParameter_TimeStamps:
		break;
	case PicamParameter_TrackFrames:
		break;
	case PicamParameter_TriggerCoupling:
		break;
	case PicamParameter_TriggerDetermination:
		break;
	case PicamParameter_TriggerFrequency:
		break;
	case PicamParameter_TriggerResponse:
		break;
	case PicamParameter_TriggerSource:
		break;
	case PicamParameter_TriggerTermination:
		break;
	case PicamParameter_TriggerThreshold:
		break;
	case PicamParameter_VerticalShiftRate:
		break;
	default:
		Picam_GetEnumerationString(PicamEnumeratedType_Parameter, parameter,
				&paramString);
		asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
				"---- Can't find parameter %s\n", paramString);
		Picam_DestroyString(paramString);
		break;
	}

	return driverParameter;

}

asynStatus ADPICam::piHandleCameraDiscovery(const PicamCameraID *id,
		PicamHandle device, PicamDiscoveryAction action) {
	PicamError error = PicamError_None;
	int status = asynSuccess;
	const char* modelString;
	asynPrint(pasynUserSelf, ASYN_TRACE_ERROR, "^^^^^^^In piDiscoverCamera\n");
	piLoadAvailableCameraIDs();
	// Take out for now.  This is making the iocCrash.  Still need to figure this out but need to start with parameters.
	piUpdateAvailableCamerasList();
	switch (action) {
	case PicamDiscoveryAction_Found:
		Picam_GetEnumerationString(PicamEnumeratedType_Model, id->model,
				&modelString);
		asynPrint(pasynUserSelf, ASYN_TRACE_ERROR, "Camera Found: %s\n",
				modelString);
		Picam_DestroyString(modelString);
		if (device != NULL) {
			PicamHandle discoveredModel;
			PicamAdvanced_GetCameraModel(device, &discoveredModel);
			printf(" discovered %s, current, %s\n", discoveredModel,
					currentCameraHandle);
			if (discoveredModel == currentCameraHandle) {
				piSetSelectedCamera(pasynUserSelf, selectedCameraIndex);
			}

		}
		break;
	case PicamDiscoveryAction_Lost:
		Picam_GetEnumerationString(PicamEnumeratedType_Model, id->model,
				&modelString);
		asynPrint(pasynUserSelf, ASYN_TRACE_ERROR, "Camera Lost: %s\n",
				modelString);
		Picam_DestroyString(modelString);

		if (device != NULL) {
			PicamHandle discoveredModel;
			PicamAdvanced_GetCameraModel(device, &discoveredModel);
			printf(" discovered %s, current, %s", discoveredModel,
					currentCameraHandle);
			if (discoveredModel == currentCameraHandle) {
				setStringParam(PICAM_CameraInterface, notAvailable);
				setStringParam(PICAM_SensorName, notAvailable);
				setStringParam(PICAM_SerialNumber, notAvailable);
				setStringParam(PICAM_FirmwareRevision, notAvailable);
				callParamCallbacks();
			}
		}
		break;
	default:
		asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
				"Unexpected discovery action%d", action);
	}
	if (device == NULL) {
		Picam_GetEnumerationString(PicamEnumeratedType_Model, id->model,
				&modelString);
		asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
				"No device found with camera\n");
		Picam_DestroyString(modelString);
	} else {
		Picam_GetEnumerationString(PicamEnumeratedType_Model, id->model,
				&modelString);
		asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
				"Device found with camera\n");

		Picam_DestroyString(modelString);

	}

	return (asynStatus) status;
}

asynStatus ADPICam::piHandleParameterRelevanceChanged(PicamHandle camera,
		PicamParameter parameter, pibln relevent) {
	PicamError error = PicamError_None;
	int status = asynSuccess;
	const char *functionName = "piHandleParameterRelevanceChanged";
	printf("parameter Relevance Changed");
	status = piSetParameterRelevance(pasynUserSelf, parameter, (int) relevent);

	return (asynStatus) status;
}

asynStatus ADPICam::piHandleParameterIntegerValueChanged(PicamHandle camera,
		PicamParameter parameter, piint value) {
	const char *functionName = "piHandleParameterIntegerValueChanged";
	PicamError error = PicamError_None;
	int status = asynSuccess;
	const char *parameterString;
	int driverParameter
	asynPrint(pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s Enter\n", driverName,
			functionName);

	driverParameter = piLookupDriverParameter(parameter);
	if (driverParameter >= 0){
		setIntegerParam(driverParameter, value);
	}
	else {
		Picam_GetEnumerationString(PicamEnumeratedType_Parameter, parameter,
				&parameterString);
		asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
				"Parameter %s integer value Changed %d."
				" This change was unhandled.\n",
				parameterString, value);
		Picam_DestroyString(parameterString);
	}
	callParamCallbacks();

	asynPrint(pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s Exit\n", driverName,
			functionName);

	return (asynStatus) status;
}

asynStatus ADPICam::piHandleParameterLargeIntegerValueChanged(
		PicamHandle camera, PicamParameter parameter, pi64s value) {
	const char *functionName = "piHandleParameterLargeIntegerValueChanged";
	PicamError error = PicamError_None;
	int status = asynSuccess;
	const char *parameterString;
	asynPrint(pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s Enter\n", driverName,
			functionName);
	Picam_GetEnumerationString(PicamEnumeratedType_Parameter, parameter,
			&parameterString);
	printf("parameter %s integer value Changed %d", parameterString, value);
	Picam_DestroyString(parameterString);

	asynPrint(pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s Exit\n", driverName,
			functionName);
	return (asynStatus) status;
}

asynStatus ADPICam::piHandleParameterFloatingPointValueChanged(
		PicamHandle camera, PicamParameter parameter, piflt value) {
	const char *functionName = "piHandleParameterFloatingPointValueChanged";
	PicamError error = PicamError_None;
	int status = asynSuccess;
	const pichar *paramString;
	int driverParameter;
	driverParameter = piLookupDriverParameter(parameter);
	if (parameter == PicamParameter_AdcSpeed){
		const PicamCollectionConstraint *speedConstraint;
		piflt fltVal;
		error = Picam_GetParameterCollectionConstraint(currentCameraHandle,
				PicamParameter_AdcSpeed,
				PicamConstraintCategory_Capable,
				&speedConstraint);
		error = Picam_GetParameterFloatingPointValue(
				currentCameraHandle, PicamParameter_AdcSpeed, &fltVal);
		printf("setting value for ADCspeed % f\n", fltVal);
		for (int ii=0; ii < speedConstraint->values_count; ii++){
			if (speedConstraint->values_array[ii] == fltVal){
				setIntegerParam(driverParameter, ii);
			}
		}

	}
	else if (driverParameter >= 0){
		Picam_GetEnumerationString(PicamEnumeratedType_Parameter, parameter,
				&paramString);
		asynPrint(pasynUserSelf, ASYN_TRACE_FLOW,
				"%s:%s Setting PICAM parameter %s to driverParameter %d, value %f\n",
				driverName,
				functionName,
				paramString,
				driverParameter,
				value);
		Picam_DestroyString(paramString);
		setDoubleParam(driverParameter, value);
	}
	else {
		Picam_GetEnumerationString(PicamEnumeratedType_Parameter, parameter,
				&paramString);
		asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
				"%s:%s Parameter %s floating point value Changed to %f.  "
				"This change is not handled.\n",
				driverName,
				functionName,
				paramString,
				value);
		Picam_DestroyString(paramString);
		switch (parameter) {
		case PicamParameter_ExposureTime:
			setDoubleParam(ADAcquireTime, value);
		}
	}
	callParamCallbacks();
	asynPrint(pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s Exit\n", driverName,
			functionName);

	return (asynStatus) status;
}

asynStatus ADPICam::piHandleParameterRoisValueChanged(PicamHandle camera,
		PicamParameter parameter, const PicamRois *value) {
	const char *functionName = "piHandleParameterRoisValueChanged";
	PicamError error = PicamError_None;
	int status = asynSuccess;
	const char *parameterString;
	Picam_GetEnumerationString(PicamEnumeratedType_Parameter, parameter,
			&parameterString);
	printf("parameter %s Rois value Changed\n", parameterString);
	asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
			"\n----- minX = %d\n----- minY = %d\n"
					"----- sizeX = %d\n----- sizeY = %d\n"
					"----- binX = %d\n----- binY = %d\n", value->roi_array[0].x,
			value->roi_array[0].y, value->roi_array[0].width,
			value->roi_array[0].height, value->roi_array[0].x_binning,
			value->roi_array[0].y_binning);
	Picam_DestroyString(parameterString);

	asynPrint(pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s Exit\n", driverName,
			functionName);

	return (asynStatus) status;
}

asynStatus ADPICam::piHandleParameterPulseValueChanged(PicamHandle camera,
		PicamParameter parameter, const PicamPulse *value) {
	const char *functionName = "piHandleParameterPulseValueChanged";
	PicamError error = PicamError_None;
	int status = asynSuccess;
	const char *parameterString;
	asynPrint(pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s Enter\n", driverName,
			functionName);
	Picam_GetEnumerationString(PicamEnumeratedType_Parameter, parameter,
			&parameterString);
	printf("parameter %s Pulse value Changed to %f\n", parameterString, value);
	Picam_DestroyString(parameterString);

	asynPrint(pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s Exit\n", driverName,
			functionName);

	return (asynStatus) status;
}

asynStatus ADPICam::piHandleParameterModulationsValueChanged(PicamHandle camera,
		PicamParameter parameter, const PicamModulations *value) {
	const char *functionName = "piHandleParameterModulationsValueChanged";
	PicamError error = PicamError_None;
	int status = asynSuccess;
	const char *parameterString;
	asynPrint(pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s Enter\n", driverName,
			functionName);
	Picam_GetEnumerationString(PicamEnumeratedType_Parameter, parameter,
			&parameterString);
	printf("parameter %s Modulations value Changed to %f", parameterString,
			value);
	Picam_DestroyString(parameterString);

	asynPrint(pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s Exit\n", driverName,
			functionName);

	return (asynStatus) status;
}

/**
 *
 */
asynStatus ADPICam::piRegisterConstraintChangeWatch(PicamHandle cameraHandle) {
	int status = asynSuccess;
	piint parameterCount;
	const PicamParameter *parameterList;
	PicamError error;

	error = Picam_GetParameters(currentCameraHandle, &parameterList,
			&parameterCount);
	if (error != PicamError_None) {
		//TODO
	}
	for (int ii = 0; ii < parameterCount; ii++) {
		error = PicamAdvanced_UnregisterForIsRelevantChanged(cameraHandle,
				parameterList[ii], piParameterRelevanceChanged);
	}
	return (asynStatus) status;
}

PicamError PIL_CALL ADPICam::piParameterRelevanceChanged(PicamHandle camera,
		PicamParameter parameter, pibln relevent) {
	int status = asynSuccess;
	PicamError error = PicamError_None;

	status = ADPICam_Instance->piHandleParameterRelevanceChanged(camera,
			parameter, relevent);

	return error;
}

PicamError PIL_CALL ADPICam::piParameterIntegerValueChanged(PicamHandle camera,
		PicamParameter parameter, piint value) {
	int status = asynSuccess;
	PicamError error = PicamError_None;
	const char *functionName = "piParameterIntegerValueChanged";

	status = ADPICam_Instance->piHandleParameterIntegerValueChanged(camera,
			parameter, value);

	return error;
}

PicamError PIL_CALL ADPICam::piParameterLargeIntegerValueChanged(
		PicamHandle camera, PicamParameter parameter, pi64s value) {
	int status = asynSuccess;
	PicamError error = PicamError_None;
	const char *functionName = "piParameterLargeIntegerValueChanged";

	status = ADPICam_Instance->piHandleParameterLargeIntegerValueChanged(camera,
			parameter, value);

	return error;
}

PicamError PIL_CALL ADPICam::piParameterFloatingPointValueChanged(
		PicamHandle camera, PicamParameter parameter, piflt value) {
	int status = asynSuccess;
	PicamError error = PicamError_None;
	const char *functionName = "piParameterFloatingPointValueChanged";

	status = ADPICam_Instance->piHandleParameterFloatingPointValueChanged(
			camera, parameter, value);

	return error;
}

PicamError PIL_CALL ADPICam::piParameterRoisValueChanged(PicamHandle camera,
		PicamParameter parameter, const PicamRois *value) {
	int status = asynSuccess;
	PicamError error = PicamError_None;
	const char *functionName = "piParameterRoisValueChanged";

	status = ADPICam_Instance->piHandleParameterRoisValueChanged(camera,
			parameter, value);

	return error;
}

PicamError PIL_CALL ADPICam::piParameterPulseValueChanged(PicamHandle camera,
		PicamParameter parameter, const PicamPulse *value) {
	int status = asynSuccess;
	PicamError error = PicamError_None;
	const char *functionName = "piParameterPulseValueChanged";

	status = ADPICam_Instance->piHandleParameterPulseValueChanged(camera,
			parameter, value);

	return error;
}

PicamError PIL_CALL ADPICam::piParameterModulationsValueChanged(
		PicamHandle camera, PicamParameter parameter,
		const PicamModulations *value) {
	int status = asynSuccess;
	PicamError error = PicamError_None;
	const char *functionName = "piParameterModulationsValueChanged";

	status = ADPICam_Instance->piHandleParameterModulationsValueChanged(camera,
			parameter, value);

	return error;
}

asynStatus ADPICam::piPrintRoisConstraints() {
	const char *functionName = "piPrintRoisConstraints";
	PicamError error = PicamError_None;
	const PicamRoisConstraint *roisConstraints;
	const char *errorString;

	error = Picam_GetParameterRoisConstraint(currentCameraHandle,
			PicamParameter_Rois, PicamConstraintCategory_Capable,
			&roisConstraints);
	if (error != PicamError_None) {
		Picam_GetEnumerationString(PicamEnumeratedType_Error, error,
				&errorString);
		asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
				"%s:%s Error retrieving rois constraints %s\n", driverName,
				functionName, errorString);
		Picam_DestroyString(errorString);
		return asynError;
	}

}
/**
 *
 */
asynStatus ADPICam::piRegisterRelevantWatch(PicamHandle cameraHandle) {
	int status = asynSuccess;
	piint parameterCount;
	const PicamParameter *parameterList;
	PicamError error;

	error = Picam_GetParameters(currentCameraHandle, &parameterList,
			&parameterCount);
	if (error != PicamError_None) {
		//TODO
	}
	for (int ii = 0; ii < parameterCount; ii++) {
		error = PicamAdvanced_RegisterForIsRelevantChanged(cameraHandle,
				parameterList[ii], piParameterRelevanceChanged);
	}
	return (asynStatus) status;
}

asynStatus ADPICam::piRegisterValueChangeWatch(PicamHandle cameraHandle) {
	int status = asynSuccess;
	piint parameterCount;
	const PicamParameter *parameterList;
	PicamValueType valueType;
	PicamError error;
	const char *functionName = "piRegisterValueChangeWatch";

	error = Picam_GetParameters(currentCameraHandle, &parameterList,
			&parameterCount);
	if (error != PicamError_None) {
		//TODO
	}
	for (int ii = 0; ii < parameterCount; ii++) {

		error = Picam_GetParameterValueType(cameraHandle, parameterList[ii],
				&valueType);
		if (error != PicamError_None) {
			asynPrint(pasynUserSelf, ASYN_TRACE_ERROR, "%s:%s \n", driverName,
					functionName);
			return asynError;
		}
		switch (valueType) {
		case PicamValueType_Integer:
		case PicamValueType_Boolean:
		case PicamValueType_Enumeration:
			error = PicamAdvanced_RegisterForIntegerValueChanged(cameraHandle,
					parameterList[ii], piParameterIntegerValueChanged);
			break;
		case PicamValueType_LargeInteger:
			error = PicamAdvanced_RegisterForLargeIntegerValueChanged(
					cameraHandle, parameterList[ii],
					piParameterLargeIntegerValueChanged);
			break;
		case PicamValueType_FloatingPoint:
			error = PicamAdvanced_RegisterForFloatingPointValueChanged(
					cameraHandle, parameterList[ii],
					piParameterFloatingPointValueChanged);
			break;
		case PicamValueType_Rois:
			error = PicamAdvanced_RegisterForRoisValueChanged(cameraHandle,
					parameterList[ii], piParameterRoisValueChanged);
			break;
		case PicamValueType_Pulse:
			error = PicamAdvanced_RegisterForPulseValueChanged(cameraHandle,
					parameterList[ii], piParameterPulseValueChanged);
			break;
		case PicamValueType_Modulations:
			error = PicamAdvanced_RegisterForModulationsValueChanged(
					cameraHandle, parameterList[ii],
					piParameterModulationsValueChanged);
			break;
		default: {
			asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
					"%s:%s Unexpected valueType %s", driverName, functionName,
					valueType);
			return asynError;
		}
			break;
		}
	}
	return (asynStatus) status;
}

asynStatus ADPICam::piSetParameterRelevance(asynUser *pasynUser,
		PicamParameter parameter, int relevence) {
	int status = asynSuccess;
	int driverParameter = -1;
	static const char *functionName = "piSetSelectedCamera";
	const pichar* string;

	switch (parameter) {
	case PicamParameter_Accumulations:
		driverParameter = PICAM_AccumulationsRelevant;
		break;
	case PicamParameter_ActiveBottomMargin:
		driverParameter = PICAM_ActiveBottomMarginRelevant;
		break;
	case PicamParameter_ActiveHeight:
		driverParameter = PICAM_ActiveHeightRelevant;
		break;
	case PicamParameter_ActiveLeftMargin:
		driverParameter = PICAM_ActiveLeftMarginRelevant;
		break;
	case PicamParameter_ActiveRightMargin:
		driverParameter = PICAM_ActiveRightMarginRelevant;
		break;
	case PicamParameter_ActiveTopMargin:
		driverParameter = PICAM_ActiveTopMarginRelevant;
		break;
	case PicamParameter_ActiveWidth:
		driverParameter = PICAM_ActiveWidthRelevant;
		break;
	case PicamParameter_AdcAnalogGain:
		driverParameter = PICAM_AdcAnalogGainRelevant;
		break;
	case PicamParameter_AdcBitDepth:
		driverParameter = PICAM_AdcBitDepthRelevant;
		break;
	case PicamParameter_AdcEMGain:
		driverParameter = PICAM_AdcEMGainRelevant;
		break;
	case PicamParameter_AdcQuality:
		driverParameter = PICAM_AdcQualityRelevant;
		break;
	case PicamParameter_AdcSpeed:
		driverParameter = PICAM_AdcSpeedRelevant;
		break;
	case PicamParameter_AuxOutput:
		driverParameter = PICAM_AuxOutputRelevant;
		break;
	case PicamParameter_BracketGating:
		driverParameter = PICAM_BracketGatingRelevant;
		break;
	case PicamParameter_CcdCharacteristics:
		driverParameter = PICAM_CcdCharacteristicsRelevant;
		break;
	case PicamParameter_CleanBeforeExposure:
		driverParameter = PICAM_CleanBeforeExposureRelevant;
		break;
	case PicamParameter_CleanCycleCount:
		driverParameter = PICAM_CleanCycleCountRelevant;
		break;
	case PicamParameter_CleanCycleHeight:
		driverParameter = PICAM_CleanCycleHeightRelevant;
		break;
	case PicamParameter_CleanSectionFinalHeight:
		driverParameter = PICAM_CleanSectionFinalHeightRelevant;
		break;
	case PicamParameter_CleanSectionFinalHeightCount:
		driverParameter = PICAM_CleanSectionFinalHeightCountRelevant;
		break;
	case PicamParameter_CleanSerialRegister:
		driverParameter = PICAM_CleanSerialRegisterRelevant;
		break;
	case PicamParameter_CleanUntilTrigger:
		driverParameter = PICAM_CleanUntilTriggerRelevant;
		break;
	case PicamParameter_CorrectPixelBias:
		driverParameter = PICAM_CorrectPixelBiasRelevant;
		break;
	case PicamParameter_CustomModulationSequence:
		driverParameter = PICAM_CustomModulationSequenceRelevant;
		break;
	case PicamParameter_DifEndingGate:
		driverParameter = PICAM_DifEndingGateRelevant;
		break;
	case PicamParameter_DifStartingGate:
		driverParameter = PICAM_DifStartingGateRelevant;
		break;
	case PicamParameter_DisableCoolingFan:
		driverParameter = PICAM_DisableCoolingFanRelevant;
		break;
	case PicamParameter_DisableDataFormatting:
		driverParameter = PICAM_DisableDataFormattingRelevant;
		break;
	case PicamParameter_EMIccdGain:
		driverParameter = PICAM_EMIccdGainRelevant;
		break;
	case PicamParameter_EMIccdGainControlMode:
		driverParameter = PICAM_EMIccdGainControlModeRelevant;
		break;
	case PicamParameter_EnableIntensifier:
		driverParameter = PICAM_EnableIntensifierRelevant;
		break;
	case PicamParameter_EnableModulation:
		driverParameter = PICAM_EnableModulationRelevant;
		break;
	case PicamParameter_EnableModulationOutputSignal:
		driverParameter = PICAM_EnableModulationOutputSignalRelevant;
		break;
	case PicamParameter_EnableNondestructiveReadout:
		driverParameter = PICAM_EnableNondestructiveReadoutRelevant;
		break;
	case PicamParameter_EnableSensorWindowHeater:
		driverParameter = PICAM_EnableSensorWindowHeaterRelevant;
		break;
	case PicamParameter_EnableSyncMaster:
		driverParameter = PICAM_EnableSyncMasterRelevant;
		break;
	case PicamParameter_ExactReadoutCountMaximum:
		driverParameter = PICAM_ExactReadoutCountMaximumRelevant;
		break;
	case PicamParameter_ExposureTime:
		driverParameter = PICAM_ExposureTimeRelevant;
		break;
	case PicamParameter_FrameRateCalculation:
		driverParameter = PICAM_FrameRateCalculationRelevant;
		break;
	case PicamParameter_FrameSize:
		driverParameter = PICAM_FrameSizeRelevant;
		break;
	case PicamParameter_FramesPerReadout:
		driverParameter = PICAM_FramesPerReadoutRelevant;
		break;
	case PicamParameter_FrameStride:
		driverParameter = PICAM_FrameStrideRelevant;
		break;
	case PicamParameter_FrameTrackingBitDepth:
		driverParameter = PICAM_FrameTrackingBitDepthRelevant;
		break;
	case PicamParameter_GateTracking:
		driverParameter = PICAM_GateTrackingRelevant;
		break;
	case PicamParameter_GateTrackingBitDepth:
		driverParameter = PICAM_GateTrackingBitDepthRelevant;
		break;
	case PicamParameter_GatingMode:
		driverParameter = PICAM_GatingModeRelevant;
		break;
	case PicamParameter_GatingSpeed:
		driverParameter = PICAM_GatingSpeedRelevant;
		break;
	case PicamParameter_IntensifierDiameter:
		driverParameter = PICAM_IntensifierDiameterRelevant;
		break;
	case PicamParameter_IntensifierGain:
		driverParameter = PICAM_IntensifierGainRelevant;
		break;
	case PicamParameter_IntensifierOptions:
		driverParameter = PICAM_IntensifierOptionsRelevant;
		break;
	case PicamParameter_IntensifierStatus:
		driverParameter = PICAM_IntensifierStatusRelevant;
		break;
	case PicamParameter_InvertOutputSignal:
		driverParameter = PICAM_InvertOutputSignalRelevant;
		break;
	case PicamParameter_KineticsWindowHeight:
		driverParameter = PICAM_KineticsWindowHeightRelevant;
		break;
	case PicamParameter_MaskedBottomMargin:
		driverParameter = PICAM_MaskedBottomMarginRelevant;
		break;
	case PicamParameter_MaskedHeight:
		driverParameter = PICAM_MaskedHeightRelevant;
		break;
	case PicamParameter_MaskedTopMargin:
		driverParameter = PICAM_MaskedTopMarginRelevant;
		break;
	case PicamParameter_ModulationDuration:
		driverParameter = PICAM_ModulationDurationRelevant;
		break;
	case PicamParameter_ModulationFrequency:
		driverParameter = PICAM_ModulationFrequencyRelevant;
		break;
	case PicamParameter_ModulationOutputSignalAmplitude:
		driverParameter = PICAM_EnableModulationOutputSignalAmplitudeRelevant;
		break;
	case PicamParameter_ModulationOutputSignalFrequency:
		driverParameter = PICAM_EnableModulationOutputSignalFrequencyRelevant;
		break;
	case PicamParameter_ModulationTracking:
		driverParameter = PICAM_ModulationTrackingRelevant;
		break;
	case PicamParameter_ModulationTrackingBitDepth:
		driverParameter = PICAM_ModulationTrackingBitDepthRelevant;
		break;
	case PicamParameter_NondestructiveReadoutPeriod:
		driverParameter = PICAM_NondestructiveReadoutPeriodRelevant;
		break;
	case PicamParameter_NormalizeOrientation:
		driverParameter = PICAM_NormalizeOrientationRelevant;
		break;
	case PicamParameter_OnlineReadoutRateCalculation:
		driverParameter = PICAM_OnlineReadoutRateCalculationRelevant;
		break;
	case PicamParameter_Orientation:
		driverParameter = PICAM_OrientationRelevant;
		break;
	case PicamParameter_OutputSignal:
		driverParameter = PICAM_OutputSignalRelevant;
		break;
	case PicamParameter_PhosphorDecayDelay:
		driverParameter = PICAM_PhosphorDecayDelayRelevant;
		break;
	case PicamParameter_PhosphorDecayDelayResolution:
		driverParameter = PICAM_PhosphorDecayDelayResolutionRelevant;
		break;
	case PicamParameter_PhosphorType:
		driverParameter = PICAM_PhosphorTypeRelevant;
		break;
	case PicamParameter_PhotocathodeSensitivity:
		driverParameter = PICAM_PhotocathodeSensitivityRelevant;
		break;
	case PicamParameter_PhotonDetectionMode:
		driverParameter = PICAM_PhotonDetectionModeRelevant;
		break;
	case PicamParameter_PhotonDetectionThreshold:
		driverParameter = PICAM_PhotonDetectionThresholdRelevant;
		break;
	case PicamParameter_PixelBitDepth:
		driverParameter = PICAM_PixelBitDepthRelevant;
		break;
	case PicamParameter_PixelFormat:
		driverParameter = PICAM_PixelFormatRelevant;
		break;
	case PicamParameter_PixelGapHeight:
		driverParameter = PICAM_PixelGapHeightRelevant;
		break;
	case PicamParameter_PixelGapWidth:
		driverParameter = PICAM_PixelGapWidthRelevant;
		break;
	case PicamParameter_PixelHeight:
		driverParameter = PICAM_PixelHeightRelevant;
		break;
	case PicamParameter_PixelWidth:
		driverParameter = PICAM_PixelWidthRelevant;
		break;
	case PicamParameter_ReadoutControlMode:
		driverParameter = PICAM_ReadoutControlModeRelevant;
		break;
	case PicamParameter_ReadoutCount:
		driverParameter = PICAM_ReadoutCountRelevant;
		break;
	case PicamParameter_ReadoutOrientation:
		driverParameter = PICAM_ReadoutOrientationRelevant;
		break;
	case PicamParameter_ReadoutPortCount:
		driverParameter = PICAM_ReadoutPortCountRelevant;
		break;
	case PicamParameter_ReadoutRateCalculation:
		driverParameter = PICAM_ReadoutRateCalculationRelevant;
		break;
	case PicamParameter_ReadoutStride:
		driverParameter = PICAM_ReadoutStrideRelevant;
		break;
	case PicamParameter_ReadoutTimeCalculation:
		driverParameter = PICAM_ReadoutTimeCalculationRelevant;
		break;
	case PicamParameter_RepetitiveGate:
		driverParameter = PICAM_RepetitiveGateRelevant;
		break;
	case PicamParameter_RepetitiveModulationPhase:
		driverParameter = PICAM_RepetitiveModulationPhaseRelevant;
		break;
	case PicamParameter_Rois:
		driverParameter = PICAM_RoisRelevant;
		break;
	case PicamParameter_SecondaryActiveHeight:
		driverParameter = PICAM_SecondaryActiveHeightRelevant;
		break;
	case PicamParameter_SecondaryMaskedHeight:
		driverParameter = PICAM_SecondaryMaskedHeightRelevant;
		break;
	case PicamParameter_SensorActiveBottomMargin:
		driverParameter = PICAM_SensorActiveBottomMarginRelevant;
		break;
	case PicamParameter_SensorActiveHeight:
		driverParameter = PICAM_SensorActiveHeightRelevant;
		break;
	case PicamParameter_SensorActiveLeftMargin:
		driverParameter = PICAM_SensorActiveLeftMarginRelevant;
		break;
	case PicamParameter_SensorActiveRightMargin:
		driverParameter = PICAM_SensorActiveRightMarginRelevant;
		break;
	case PicamParameter_SensorActiveTopMargin:
		driverParameter = PICAM_SensorActiveTopMarginRelevant;
		break;
	case PicamParameter_SensorActiveWidth:
		driverParameter = PICAM_SensorActiveWidthRelevant;
		break;
	case PicamParameter_SensorMaskedBottomMargin:
		driverParameter = PICAM_SensorMaskedBottomMarginRelevant;
		break;
	case PicamParameter_SensorMaskedHeight:
		driverParameter = PICAM_SensorMaskedHeightRelevant;
		break;
	case PicamParameter_SensorMaskedTopMargin:
		driverParameter = PICAM_SensorMaskedTopMarginRelevant;
		break;
	case PicamParameter_SensorSecondaryActiveHeight:
		driverParameter = PICAM_SensorSecondaryActiveHeightRelevant;
		break;
	case PicamParameter_SensorSecondaryMaskedHeight:
		driverParameter = PICAM_SensorSecondaryMaskedHeightRelevant;
		break;
	case PicamParameter_SensorTemperatureReading:
		driverParameter = PICAM_SensorTemperatureReadingRelevant;
		break;
	case PicamParameter_SensorTemperatureSetPoint:
		driverParameter = PICAM_SensorTemperatureSetPointRelevant;
		break;
	case PicamParameter_SensorTemperatureStatus:
		driverParameter = PICAM_SensorTemperatureStatusRelevant;
		break;
	case PicamParameter_SensorType:
		driverParameter = PICAM_SensorTypeRelevant;
		break;
	case PicamParameter_SequentialEndingGate:
		driverParameter = PICAM_SequentialEndingGateRelevant;
		break;
	case PicamParameter_SequentialEndingModulationPhase:
		driverParameter = PICAM_SequentialEndingModulationPhaseRelevant;
		break;
	case PicamParameter_SequentialGateStepCount:
		driverParameter = PICAM_SequentialGateStepCountRelevant;
		break;
	case PicamParameter_SequentialGateStepIterations:
		driverParameter = PICAM_SequentialGateStepIterationsRelevant;
		break;
	case PicamParameter_SequentialStartingGate:
		driverParameter = PICAM_SequentialStartingGateRelevant;
		break;
	case PicamParameter_SequentialStartingModulationPhase:
		driverParameter = PICAM_SequentialStartingModulationPhaseRelevant;
		break;
	case PicamParameter_ShutterClosingDelay:
		driverParameter = PICAM_ShutterClosingDelayRelevant;
		break;
	case PicamParameter_ShutterDelayResolution:
		driverParameter = PICAM_ShutterDelayResolutionRelevant;
		break;
	case PicamParameter_ShutterOpeningDelay:
		driverParameter = PICAM_ShutterOpeningDelayRelevant;
		break;
	case PicamParameter_ShutterTimingMode:
		driverParameter = PICAM_ShutterTimingModeRelevant;
		break;
	case PicamParameter_SyncMaster2Delay:
		driverParameter = PICAM_SyncMaster2DelayRelevant;
		break;
	case PicamParameter_TimeStampBitDepth:
		driverParameter = PICAM_TimeStampBitDepthRelevant;
		break;
	case PicamParameter_TimeStampResolution:
		driverParameter = PICAM_TimeStampResolutionRelevant;
		break;
	case PicamParameter_TimeStamps:
		driverParameter = PICAM_TimeStampsRelevant;
		break;
	case PicamParameter_TrackFrames:
		driverParameter = PICAM_TrackFramesRelevant;
		break;
	case PicamParameter_TriggerCoupling:
		driverParameter = PICAM_TriggerCouplingRelevant;
		break;
	case PicamParameter_TriggerDetermination:
		driverParameter = PICAM_TriggerDeterminationRelevant;
		break;
	case PicamParameter_TriggerFrequency:
		driverParameter = PICAM_TriggerFrequencyRelevant;
		break;
	case PicamParameter_TriggerResponse:
		driverParameter = PICAM_TriggerResponseRelevant;
		break;
	case PicamParameter_TriggerSource:
		driverParameter = PICAM_TriggerSourceRelevant;
		break;
	case PicamParameter_TriggerTermination:
		driverParameter = PICAM_TriggerTerminationRelevant;
		break;
	case PicamParameter_TriggerThreshold:
		driverParameter = PICAM_TriggerThresholdRelevant;
		break;
	case PicamParameter_VerticalShiftRate:
		driverParameter = PICAM_VerticalShiftRateRelevant;
		break;
	default:
		Picam_GetEnumerationString(PicamEnumeratedType_Parameter, parameter,
				&string);
		asynPrint(pasynUser, ASYN_TRACE_ERROR, "---- Can't find parameter %s\n",
				string);
		Picam_DestroyString(string);
		break;
	}
	setIntegerParam(driverParameter, relevence);
	return (asynStatus) status;
}

asynStatus ADPICam::piSetParameterValuesFromSelectedCamera() {
	int status = asynSuccess;
	PicamError error;
	const pichar *errorString;
	const pichar *paramString;
	piint parameterCount;
	const PicamParameter *parameterList;
	static const char *functionName = "piSetParameterValuesFromSelectedCamera";
	int driverParam = -1;
	PicamValueType paramType;

	asynPrint(pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s Enter\n", driverName,
			functionName);

	Picam_GetParameters(currentCameraHandle, &parameterList, &parameterCount);
	asynPrint(pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s %d parameters found\n",
			driverName,
			functionName,
			parameterCount);

	for (int ii = 0; ii < parameterCount; ii++) {
		Picam_GetEnumerationString(PicamEnumeratedType_Parameter,
				parameterList[ii], &paramString);
		asynPrint(pasynUserSelf, ASYN_TRACE_FLOW, "---- Found %s\n",
				paramString);
		Picam_DestroyString(paramString);
		driverParam = piLookupDriverParameter(parameterList[ii]);
		if (driverParam >= 0) {
			error = Picam_GetParameterValueType(currentCameraHandle,
					parameterList[ii], &paramType);
			if (error != PicamError_None) {
				//TODO
			}
			switch (paramType) {
			case PicamValueType_Integer:
			case PicamValueType_Enumeration:
				piint intVal;
				error = Picam_GetParameterIntegerValue(currentCameraHandle,
						parameterList[ii], &intVal);
				setIntegerParam(driverParam, intVal);
				break;
			case PicamValueType_FloatingPoint:
				piflt fltVal;
				const PicamCollectionConstraint *speedConstraint;
				if (parameterList[ii] == PicamParameter_AdcSpeed){
					error = Picam_GetParameterCollectionConstraint(currentCameraHandle,
							PicamParameter_AdcSpeed,
							PicamConstraintCategory_Capable,
							&speedConstraint);
					error = Picam_GetParameterFloatingPointValue(
							currentCameraHandle, PicamParameter_AdcSpeed, &fltVal);
					for (int ii=0; ii < speedConstraint->values_count; ii++){
						if (speedConstraint->values_array[ii] == fltVal){
							setIntegerParam(driverParam, ii);
						}
					}
				}
				else {
					error = Picam_GetParameterFloatingPointValue(
							currentCameraHandle, parameterList[ii], &fltVal);
					setDoubleParam(driverParam, fltVal);

				}
				break;
			}

		} else if (parameterList[ii] == PicamParameter_Rois) {
			const PicamRois *paramRois;
			error = Picam_GetParameterRoisValue(currentCameraHandle,
					parameterList[ii], &paramRois);
			asynPrint(pasynUserSelf, ASYN_TRACE_ERROR, "Rois %d\n",
					paramRois->roi_count);

			if (paramRois->roi_count == 1) {
				setIntegerParam(ADBinX, paramRois->roi_array[0].x_binning);
				setIntegerParam(ADBinY, paramRois->roi_array[0].y_binning);
				setIntegerParam(ADMinX, paramRois->roi_array[0].x);
				setIntegerParam(ADMinY, paramRois->roi_array[0].y);
				setIntegerParam(ADSizeX, paramRois->roi_array[0].width);
				setIntegerParam(ADSizeY, paramRois->roi_array[0].height);
				setIntegerParam(NDArraySizeX, paramRois->roi_array[0].width);
				setIntegerParam(NDArraySizeY, paramRois->roi_array[0].height);
			}
		}
	}


	Picam_DestroyParameters(parameterList);
	callParamCallbacks();

	asynPrint(pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s Exit\n", driverName,
			functionName);

	return (asynStatus) status;
}

/**
 Set values associated with selected detector
 */
asynStatus ADPICam::piSetRois(int minX, int minY, int width, int height,
		int binX, int binY) {
	int status = asynSuccess;
	const PicamRois *rois;
	const PicamRoisConstraint *roisConstraints;
	const char *functionName = "piSetRois";\
	const pichar *errorString;

	PicamError error = PicamError_None;

	error = Picam_GetParameterRoisValue(currentCameraHandle,
			PicamParameter_Rois, &rois);
	if (error != PicamError_None) {
		Picam_GetEnumerationString(PicamEnumeratedType_Error, error,
				&errorString);
		asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
				"%s:%s Error retrieving rois %s\n", driverName, functionName,
				errorString);
		Picam_DestroyString(errorString);
		return asynError;
	}
	error = Picam_GetParameterRoisConstraint(currentCameraHandle,
			PicamParameter_Rois, PicamConstraintCategory_Capable,
			&roisConstraints);
	if (error != PicamError_None) {
		Picam_GetEnumerationString(PicamEnumeratedType_Error, error,
				&errorString);
		asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
				"%s:%s Error retrieving rois constraints %s\n", driverName,
				functionName, errorString);
		Picam_DestroyString(errorString);
		return asynError;
	}
	if (rois->roi_count == 1) {
		PicamRoi *roi = &(rois->roi_array[0]);
		bool allInRange = true;
		if (minX >= roisConstraints->x_constraint.minimum
				&& minX <= roisConstraints->x_constraint.maximum) {
			roi->x = minX;
			setIntegerParam(ADMinX, minX);
		} else {
			allInRange = false;
		}
		if (minY >= roisConstraints->y_constraint.minimum
				&& minY <= roisConstraints->y_constraint.maximum) {
			roi->y = minY;
			setIntegerParam(ADMinY, minY);
		} else {
			allInRange = false;
		}
		if (width >= roisConstraints->width_constraint.minimum
				&& width <= roisConstraints->width_constraint.maximum) {
			printf("widthmin : %d, widthmax : %d\n",
					roisConstraints->width_constraint.minimum,
					roisConstraints->width_constraint.maximum);
			roi->width = width;
			setIntegerParam(ADSizeX, width);
		} else {
			allInRange = false;
		}
		if (height >= roisConstraints->height_constraint.minimum
				&& height <= roisConstraints->height_constraint.maximum) {
			roi->height = height;
			setIntegerParam(ADSizeY, height);

		} else {
			allInRange = false;
		}
		error = Picam_SetParameterRoisValue(currentCameraHandle,
				PicamParameter_Rois, rois);
		if (error != PicamError_None) {
			Picam_GetEnumerationString(PicamEnumeratedType_Error, error,
					&errorString);
			asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
					"%s:%s Error writing rois %s\n", driverName, functionName,
					errorString);
			Picam_DestroyString(errorString);
			return asynError;
		}
	}

	return (asynStatus) status;
}

asynStatus ADPICam::piSetSelectedCamera(asynUser *pasynUser,
		int selectedIndex) {
	int status = asynSuccess;
	const PicamFirmwareDetail *firmwareDetails;
	PicamError error;
	piint numFirmwareDetails = 0;
	const char *modelString;
	const char *interfaceString;
	static const char *functionName = "piSetSelectedCamera";
	char enumString[64];
	char firmwareString[64];

	asynPrint(pasynUser, ASYN_TRACE_FLOW,
			"%s:%s: Selected camera value=%d\n",
			driverName, functionName, selectedIndex);
	piUnregisterRelevantWatch(currentCameraHandle);
	piUnregisterConstraintChangeWatch(currentCameraHandle);
	piUnregisterValueChangeWatch(currentCameraHandle);

	if (selectedCameraIndex >= 0) {
		error = Picam_CloseCamera(currentCameraHandle);
		asynPrint(pasynUser, ASYN_TRACE_FLOW, "Picam_CloseCameraError %d\n",
				error);
	}
	asynPrint(pasynUser, ASYN_TRACE_FLOW,
			"%s:%s: Number of available cameras=%d\n", driverName, functionName,
			availableCamerasCount);
	for (int ii = 0; ii < availableCamerasCount; ii++) {
		asynPrint(pasynUser, ASYN_TRACE_FLOW, "Available Camera[%d]\n", ii);
		Picam_GetEnumerationString(PicamEnumeratedType_Model,
				(piint) availableCameraIDs[ii].model, &modelString);
		sprintf(enumString, "%s", modelString);
		asynPrint(pasynUser, ASYN_TRACE_FLOW, "\n---%s\n---%d\n---%s\n---%s\n",
				modelString, availableCameraIDs[ii].computer_interface,
				availableCameraIDs[ii].sensor_name,
				availableCameraIDs[ii].serial_number);
		Picam_DestroyString(modelString);
		//PicamAdvanced_SetUserState(availableCameraIDs[ii].model, this);
	}
	if (selectedIndex < availableCamerasCount) {

		selectedCameraIndex = selectedIndex;
		error = Picam_OpenCamera(&(availableCameraIDs[selectedIndex]),
				&currentCameraHandle);
		if (error != PicamError_None) {
			asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
					"%s:%s Trouble Opening Camera\n", driverName, functionName);
		}
		Picam_GetEnumerationString(PicamEnumeratedType_Model,
				(piint) availableCameraIDs[selectedIndex].model, &modelString);
		sprintf(enumString, "%s", modelString);
		asynPrint(pasynUser, ASYN_TRACE_FLOW,
				"%s:%s: Selected camera value=%d, %s\n", driverName,
				functionName, selectedIndex, modelString);
		Picam_DestroyString(modelString);

		Picam_GetEnumerationString(PicamEnumeratedType_ComputerInterface,
				(piint) availableCameraIDs[selectedIndex].computer_interface,
				&interfaceString);
		sprintf(enumString, "%s", interfaceString);
		status |= setStringParam(PICAM_CameraInterface, enumString);
		Picam_DestroyString(interfaceString);

		status |= setIntegerParam(PICAM_AvailableCameras, selectedIndex);
		status |= setStringParam(PICAM_SensorName,
				availableCameraIDs[selectedIndex].sensor_name);
		status |= setStringParam(PICAM_SerialNumber,
				availableCameraIDs[selectedIndex].serial_number);

		Picam_GetFirmwareDetails(&(availableCameraIDs[selectedIndex]),
				&firmwareDetails, &numFirmwareDetails);
		asynPrint(pasynUser, ASYN_TRACE_FLOW, "----%d\n", numFirmwareDetails);
		if (numFirmwareDetails > 0) {
			asynPrint(pasynUser, ASYN_TRACE_FLOW, "%s\n",
					firmwareDetails[0].detail);

			sprintf(firmwareString, "%s", firmwareDetails[0].detail);

			Picam_DestroyFirmwareDetails(firmwareDetails);
			status |= setStringParam(PICAM_FirmwareRevision, firmwareString);
		} else {
			status |= setStringParam(PICAM_FirmwareRevision, "N/A");
		}
	} else {
		setIntegerParam(PICAM_AvailableCameras, 0);
		setIntegerParam(PICAM_UnavailableCameras, 0);

	}
	callParamCallbacks();

	//piSetParameterValuesFromSelectedCamera();

	piRegisterValueChangeWatch(currentCameraHandle);
//    const char sensorName[PicamStringSize_SensorName];
//    status |= getStringParam(PICAM_SensorName, (int)PicamStringSize_SensorName, sensorName);
//    std::string sensNameStr = std::string(sensorName);
	/* Do callbacks so higher layers see any changes */

	callParamCallbacks();

	status |= piClearParameterRelevance(pasynUser);
	status |= piUpdateParameterRelevance(pasynUser);
	return (asynStatus) status;
}

asynStatus ADPICam::piSetSelectedUnavailableCamera(asynUser *pasynUser,
		int selectedIndex) {
	int status = asynSuccess;
	const PicamFirmwareDetail *firmwareDetails;
	piint numFirmwareDetails = 0;
	const char *modelString;
	const char *interfaceString;
	static const char *functionName = "piSetSelectedUnavailableCamera";
	char enumString[64];
	char firmwareString[64];

	asynPrint(pasynUser, ASYN_TRACE_FLOW, "%s:%s: Entry\n", driverName,
			functionName);
	asynPrint(pasynUser, ASYN_TRACE_FLOW, "%s:%s: Selected camera value=%d\n",
			driverName, functionName, selectedIndex);
	if (unavailableCamerasCount == 0) {
		asynPrint(pasynUser, ASYN_TRACE_WARNING,
				"%s:%s: There are no unavailable cameras\n", driverName,
				functionName);
		status |= setStringParam(PICAM_CameraInterfaceUnavailable, enumString);
		status |= setStringParam(PICAM_SensorNameUnavailable, notAvailable);
		status |= setStringParam(PICAM_SerialNumberUnavailable, notAvailable);
		status |= setStringParam(PICAM_FirmwareRevisionUnavailable,
				notAvailable);
		return asynSuccess;
	}
	asynPrint(pasynUser, ASYN_TRACE_FLOW,
			"%s:%s: Number of available cameras=%d\n", driverName, functionName,
			unavailableCamerasCount);

	Picam_GetEnumerationString(PicamEnumeratedType_Model,
			(piint) unavailableCameraIDs[selectedIndex].model, &modelString);
	sprintf(enumString, "%s", modelString);
	asynPrint(pasynUser, ASYN_TRACE_ERROR,
			"%s:%s: Selected camera value=%d, %s\n", driverName, functionName,
			selectedIndex, modelString);
	Picam_DestroyString(modelString);

	Picam_GetEnumerationString(PicamEnumeratedType_ComputerInterface,
			(piint) unavailableCameraIDs[selectedIndex].computer_interface,
			&interfaceString);
	sprintf(enumString, "%s", interfaceString);
	status |= setStringParam(PICAM_CameraInterfaceUnavailable, enumString);
	Picam_DestroyString(interfaceString);

	status |= setIntegerParam(PICAM_AvailableCameras, selectedIndex);
	status |= setStringParam(PICAM_SensorNameUnavailable,
			unavailableCameraIDs[selectedIndex].sensor_name);
	status |= setStringParam(PICAM_SerialNumberUnavailable,
			unavailableCameraIDs[selectedIndex].serial_number);

	Picam_GetFirmwareDetails(&(unavailableCameraIDs[selectedIndex]),
			&firmwareDetails, &numFirmwareDetails);
	asynPrint(pasynUser, ASYN_TRACE_FLOW, "----%d\n", numFirmwareDetails);
	if (numFirmwareDetails > 0) {
		asynPrint(pasynUser, ASYN_TRACE_FLOW, "%s\n",
				firmwareDetails[0].detail);

		sprintf(firmwareString, "%s", firmwareDetails[0].detail);

		Picam_DestroyFirmwareDetails(firmwareDetails);
		status |= setStringParam(PICAM_FirmwareRevisionUnavailable,
				firmwareString);
	} else {
		status |= setStringParam(PICAM_FirmwareRevisionUnavailable, "N/A");
	}

	asynPrint(pasynUser, ASYN_TRACE_FLOW, "%s:%s: Entry\n", driverName,
			functionName);
	return (asynStatus) status;
}

/**
 *
 */
asynStatus ADPICam::piUnregisterConstraintChangeWatch(
		PicamHandle cameraHandle) {
	int status = asynSuccess;
	piint parameterCount;
	const PicamParameter *parameterList;
	PicamError error;

	error = Picam_GetParameters(currentCameraHandle, &parameterList,
			&parameterCount);
	if (error != PicamError_None) {
		//TODO
	}
	for (int ii = 0; ii < parameterCount; ii++) {
		error = PicamAdvanced_UnregisterForIsRelevantChanged(cameraHandle,
				parameterList[ii], piParameterRelevanceChanged);
	}
	return (asynStatus) status;
}

asynStatus ADPICam::piUnregisterRelevantWatch(PicamHandle cameraHandle) {
	int status = asynSuccess;
	piint parameterCount;
	const PicamParameter *parameterList;
	PicamError error;

	error = Picam_GetParameters(currentCameraHandle, &parameterList,
			&parameterCount);
	if (error != PicamError_None) {
		//TODO
	}
	for (int ii = 0; ii < parameterCount; ii++) {
		error = PicamAdvanced_UnregisterForIsRelevantChanged(cameraHandle,
				parameterList[ii], piParameterRelevanceChanged);
	}
	return (asynStatus) status;
}

asynStatus ADPICam::piUnregisterValueChangeWatch(PicamHandle cameraHandle) {
	int status = asynSuccess;
	piint parameterCount;
	const PicamParameter *parameterList;
	PicamValueType valueType;
	PicamError error;
	const char *functionName = "piUnregisterValueChangeWatch";

	error = Picam_GetParameters(currentCameraHandle, &parameterList,
			&parameterCount);
	if (error != PicamError_None) {
		//TODO
	}
	for (int ii = 0; ii < parameterCount; ii++) {

		error = Picam_GetParameterValueType(cameraHandle, parameterList[ii],
				&valueType);
		if (error != PicamError_None) {
			asynPrint(pasynUserSelf, ASYN_TRACE_ERROR, "%s:%s \n", driverName,
					functionName);
			return asynError;
		}
		switch (valueType) {
		case PicamValueType_Integer:
		case PicamValueType_Boolean:
		case PicamValueType_Enumeration:
			error = PicamAdvanced_UnregisterForIntegerValueChanged(cameraHandle,
					parameterList[ii], piParameterIntegerValueChanged);
			break;
		case PicamValueType_LargeInteger:
			error = PicamAdvanced_UnregisterForLargeIntegerValueChanged(
					cameraHandle, parameterList[ii],
					piParameterLargeIntegerValueChanged);
			break;
		case PicamValueType_FloatingPoint:
			error = PicamAdvanced_UnregisterForFloatingPointValueChanged(
					cameraHandle, parameterList[ii],
					piParameterFloatingPointValueChanged);
			break;
		case PicamValueType_Rois:
			error = PicamAdvanced_UnregisterForRoisValueChanged(cameraHandle,
					parameterList[ii], piParameterRoisValueChanged);
			break;
		case PicamValueType_Pulse:
			error = PicamAdvanced_UnregisterForPulseValueChanged(cameraHandle,
					parameterList[ii], piParameterPulseValueChanged);
			break;
		case PicamValueType_Modulations:
			error = PicamAdvanced_UnregisterForModulationsValueChanged(
					cameraHandle, parameterList[ii],
					piParameterModulationsValueChanged);
			break;
		default: {
			asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
					"%s:%s Unexpected valueType %s", driverName, functionName,
					valueType);
			return asynError;
		}
			break;
		}
	}
	return (asynStatus) status;
}

/**
 Update PICAM parameter relevance for the current detector
 */
asynStatus ADPICam::piUpdateParameterRelevance(asynUser *pasynUser) {
	int status = asynSuccess;
	piint parameterCount;
	const PicamParameter *parameterList;
	const pichar* string;
	Picam_GetParameters(currentCameraHandle, &parameterList, &parameterCount);
	asynPrint(pasynUser, ASYN_TRACE_ERROR, "%d parameters found\n",
			parameterCount);

	for (int ii = 0; ii < parameterCount; ii++) {
		Picam_GetEnumerationString(PicamEnumeratedType_Parameter,
				parameterList[ii], &string);
		asynPrint(pasynUser, ASYN_TRACE_FLOW, "---- Found %s\n", string);
		Picam_DestroyString(string);
		status |= piSetParameterRelevance(pasynUser, parameterList[ii], 1);
	}
	Picam_DestroyParameters(parameterList);
	callParamCallbacks();
	return (asynStatus) status;
}

asynStatus ADPICam::piUpdateAvailableCamerasList() {
	int status = asynSuccess;
	const char *functionName = "piUpdateAvailableCamerasList";
	const char *modelString;
	char enumString[64];
	char *strings[MAX_ENUM_STATES];
	int values[MAX_ENUM_STATES];
	int severities[MAX_ENUM_STATES];
	//size_t nElements;
	size_t nIn;

	nIn = 0;
	asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
			"%s:%s availableCamerasCount %d\n", driverName, functionName,
			availableCamerasCount);
	for (int ii = 0; ii < availableCamerasCount; ii++) {
		Picam_GetEnumerationString(PicamEnumeratedType_Model,
				(piint) availableCameraIDs[ii].model, &modelString);
		pibln camConnected = false;
		Picam_IsCameraIDConnected(availableCameraIDs, &camConnected);
		sprintf(enumString, "%s", modelString);
		asynPrint(pasynUserSelf, ASYN_TRACE_FLOW,
				"\n%s:%s: \nCamera[%d]\n---%s\n---%d\n---%s\n---%s\n",
				driverName, functionName, ii, modelString,
				availableCameraIDs[ii].computer_interface,
				availableCameraIDs[ii].sensor_name,
				availableCameraIDs[ii].serial_number);
		if (strings[nIn])
			free(strings[nIn]);

		printf("HELLO");
		Picam_DestroyString(modelString);
		strings[nIn] = epicsStrDup(enumString);
		values[nIn] = ii;
		severities[nIn] = 0;
		(nIn)++;
	}

	doCallbacksEnum(strings, values, severities, nIn, PICAM_AvailableCameras,
			0);
	return (asynStatus) status;
}

asynStatus ADPICam::piUpdateUnavailableCamerasList() {
	int status = asynSuccess;

	return (asynStatus) status;

}

/* Code for iocsh registration */

/* PICamConfig */
static const iocshArg PICamConfigArg0 = { "Port name", iocshArgString };
static const iocshArg PICamConfigArg1 = { "maxBuffers", iocshArgInt };
static const iocshArg PICamConfigArg2 = { "maxMemory", iocshArgInt };
static const iocshArg PICamConfigArg3 = { "priority", iocshArgInt };
static const iocshArg PICamConfigArg4 = { "stackSize", iocshArgInt };
static const iocshArg * const PICamConfigArgs[] = { &PICamConfigArg0,
		&PICamConfigArg1, &PICamConfigArg2, &PICamConfigArg3, &PICamConfigArg4 };

static const iocshFuncDef configPICam = { "PICamConfig", 5, PICamConfigArgs };

static const iocshArg PICamAddDemoCamArg0 = { "Demo Camera name", iocshArgString };
static const iocshArg * const PICamAddDemoCamArgs[] = { &PICamAddDemoCamArg0 };

static const iocshFuncDef addDemoCamPICam = { "PICamAddDemoCamera", 1, PICamAddDemoCamArgs };

static void configPICamCallFunc(const iocshArgBuf *args) {
	PICamConfig(args[0].sval, args[1].ival, args[2].ival, args[3].ival,
			args[4].ival);
}

static void addDemoCamPICamCallFunc(const iocshArgBuf *args) {
	PICamAddDemoCamera(args[0].sval);
}

static void PICamRegister(void) {
	iocshRegister(&configPICam, configPICamCallFunc);
	iocshRegister(&addDemoCamPICam, addDemoCamPICamCallFunc);
}

extern "C" {
epicsExportRegistrar(PICamRegister);
}