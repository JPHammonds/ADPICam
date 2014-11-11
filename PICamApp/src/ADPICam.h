/* PICam.h
 *
 * This is an areaDetector driver for cameras that communicate
 * with the Priceton Instruments PICAM driver library
 *
 */
#ifndef ADPICAM_H
#define ADPICAM_H

#include "picam_advanced.h"
#include "ADDriver.h"
#include <iocsh.h>
#include <epicsExport.h>
#include <epicsString.h>


class ADPICam : public ADDriver
{
public:
    static const char *notAvailable;
    static const char *driverName;

	ADPICam(const char *portName,
        int maxBuffers, size_t maxMemory,
        int priority, int stackSize);
    ~ADPICam();
    /* These are the methods that we override from ADDriver */
    virtual asynStatus 	readEnum(asynUser *pasynUser,
        char *strings[],
        int values[],
        int severities[],
        size_t nElements,
        size_t *nIn);
    static asynStatus piAddDemoCamera(const char *demoCameraName);
    static PicamError PIL_CALL piCameraDiscovered(
        const PicamCameraID *id,
        PicamHandle device,
        PicamDiscoveryAction action);
    asynStatus piHandleCameraDiscovery(const PicamCameraID *id, PicamHandle device, PicamDiscoveryAction);
    asynStatus piHandleParameterRelevanceChanged(
    	PicamHandle camera,
		PicamParameter parameter,
		pibln relevent );
    asynStatus piHandleParameterIntegerValueChanged(
    	PicamHandle camera,
		PicamParameter parameter,
		piint value );
    asynStatus piHandleParameterLargeIntegerValueChanged(
    	PicamHandle camera,
		PicamParameter parameter,
		pi64s value );
    asynStatus piHandleParameterFloatingPointValueChanged(
    	PicamHandle camera,
		PicamParameter parameter,
		piflt value );
    asynStatus piHandleParameterRoisValueChanged(
    	PicamHandle camera,
		PicamParameter parameter,
		const PicamRois *value );
    asynStatus piHandleParameterPulseValueChanged(
    	PicamHandle camera,
		PicamParameter parameter,
		const PicamPulse *value );
    asynStatus piHandleParameterModulationsValueChanged(
    	PicamHandle camera,
		PicamParameter parameter,
		const PicamModulations *value );
    asynStatus piLoadAvailableCameraIDs();
    asynStatus piPrintRoisConstraints();
    static PicamError PIL_CALL piParameterRelevanceChanged(
    	PicamHandle camera,
		PicamParameter parameter,
		pibln relevent );
    static PicamError PIL_CALL piParameterFloatingPointValueChanged(
    	PicamHandle camera,
		PicamParameter parameter,
		piflt value );
    static PicamError PIL_CALL piParameterIntegerValueChanged(
    	PicamHandle camera,
		PicamParameter parameter,
		piint value );
    static PicamError PIL_CALL piParameterLargeIntegerValueChanged(
    	PicamHandle camera,
		PicamParameter parameter,
		pi64s value );
    static PicamError PIL_CALL piParameterRoisValueChanged(
    	PicamHandle camera,
		PicamParameter parameter,
		const PicamRois *value );
    static PicamError PIL_CALL piParameterPulseValueChanged(
    	PicamHandle camera,
		PicamParameter parameter,
		const PicamPulse *value );
    static PicamError PIL_CALL piParameterModulationsValueChanged(
    	PicamHandle camera,
		PicamParameter parameter,
		const PicamModulations *value );
    void report(FILE *fp, int details);
    virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
    virtual asynStatus writeFloat64(asynUser *pasynUser, epicsFloat64 value);


protected:
    int PICAM_VersionNumber;
#define PICAM_FIRST_PARAM PICAM_VersionNumber
    int PICAM_AvailableCameras;
    int PICAM_CameraInterface;
    int PICAM_SensorName;
    int PICAM_SerialNumber;
    int PICAM_FirmwareRevision;

    int PICAM_UnavailableCameras;
    int PICAM_CameraInterfaceUnavailable;
    int PICAM_SensorNameUnavailable;
    int PICAM_SerialNumberUnavailable;
    int PICAM_FirmwareRevisionUnavailable;

    int PICAM_AdcAnalogGain;
    int PICAM_AdcBitDepth;
    int PICAM_AdcEMGain;
    int PICAM_AdcQuality;
    int PICAM_AdcSpeed;
    int PICAM_CorrectPixelBias;

    int PICAM_ReadoutTimeCalc;
    int PICAM_ReadoutRateCalc;
    int PICAM_FrameRateCalc;
    int PICAM_OnlineReadoutRateCalc;

    int PICAM_ExposureTimeRelevant;
    int PICAM_ShutterClosingDelayRelevant;
    int PICAM_ShutterDelayResolutionRelevant;
    int PICAM_ShutterOpeningDelayRelevant;
    int PICAM_ShutterTimingModeRelevant;
    int PICAM_BracketGatingRelevant;
    int PICAM_CustomModulationSequenceRelevant;
    int PICAM_DifEndingGateRelevant;
    int PICAM_DifStartingGateRelevant;
    int PICAM_EMIccdGainRelevant;
    int PICAM_EMIccdGainControlModeRelevant;
    int PICAM_EnableIntensifierRelevant;
    int PICAM_EnableModulationRelevant;
    int PICAM_GatingModeRelevant;
    int PICAM_GatingSpeedRelevant;
    int PICAM_IntensifierDiameterRelevant;
    int PICAM_IntensifierGainRelevant;
    int PICAM_IntensifierOptionsRelevant;
    int PICAM_IntensifierStatusRelevant;
    int PICAM_ModulationDurationRelevant;
    int PICAM_ModulationFrequencyRelevant;
    int PICAM_PhosphorDecayDelayRelevant;
    int PICAM_PhosphorDecayDelayResolutionRelevant;
    int PICAM_PhosphorTypeRelevant;
    int PICAM_PhotocathodeSensitivityRelevant;
    int PICAM_RepetitiveGateRelevant;
    int PICAM_RepetitiveModulationPhaseRelevant;
    int PICAM_SequentialStartingModulationPhaseRelevant;
    int PICAM_SequentialEndingModulationPhaseRelevant;
    int PICAM_SequentialEndingGateRelevant;
    int PICAM_SequentialGateStepCountRelevant;
    int PICAM_SequentialGateStepIterationsRelevant;
    int PICAM_SequentialStartingGateRelevant;
    int PICAM_AdcAnalogGainRelevant;
    int PICAM_AdcBitDepthRelevant;
    int PICAM_AdcEMGainRelevant;
    int PICAM_AdcQualityRelevant;
    int PICAM_AdcSpeedRelevant;
    int PICAM_CorrectPixelBiasRelevant;
    int PICAM_AuxOutputRelevant;
    int PICAM_EnableModulationOutputSignalRelevant;
    int PICAM_EnableModulationOutputSignalFrequencyRelevant;
    int PICAM_EnableModulationOutputSignalAmplitudeRelevant;
    int PICAM_EnableSyncMasterRelevant;
    int PICAM_InvertOutputSignalRelevant;
    int PICAM_OutputSignalRelevant;
    int PICAM_SyncMaster2DelayRelevant;
    int PICAM_TriggerCouplingRelevant;
    int PICAM_TriggerDeterminationRelevant;
    int PICAM_TriggerFrequencyRelevant;
    int PICAM_TriggerResponseRelevant;
    int PICAM_TriggerSourceRelevant;
    int PICAM_TriggerTerminationRelevant;
    int PICAM_TriggerThresholdRelevant;
    int PICAM_AccumulationsRelevant;
    int PICAM_EnableNondestructiveReadoutRelevant;
    int PICAM_KineticsWindowHeightRelevant;
    int PICAM_NondestructiveReadoutPeriodRelevant;
    int PICAM_ReadoutControlModeRelevant;
    int PICAM_ReadoutOrientationRelevant;
    int PICAM_ReadoutPortCountRelevant;
    int PICAM_ReadoutTimeCalculationRelevant;
    int PICAM_VerticalShiftRateRelevant;
    int PICAM_DisableDataFormattingRelevant;
    int PICAM_ExactReadoutCountMaximumRelevant;
    int PICAM_FrameRateCalculationRelevant;
    int PICAM_FrameSizeRelevant;
    int PICAM_FramesPerReadoutRelevant;
    int PICAM_FrameStrideRelevant;
    int PICAM_FrameTrackingBitDepthRelevant;
    int PICAM_GateTrackingRelevant;
    int PICAM_GateTrackingBitDepthRelevant;
    int PICAM_ModulationTrackingRelevant;
    int PICAM_ModulationTrackingBitDepthRelevant;
    int PICAM_NormalizeOrientationRelevant;
    int PICAM_OnlineReadoutRateCalculationRelevant;
    int PICAM_OrientationRelevant;
    int PICAM_PhotonDetectionModeRelevant;
    int PICAM_PhotonDetectionThresholdRelevant;
    int PICAM_PixelBitDepthRelevant;
    int PICAM_PixelFormatRelevant;
    int PICAM_ReadoutCountRelevant;
    int PICAM_ReadoutRateCalculationRelevant;
    int PICAM_ReadoutStrideRelevant;
    int PICAM_RoisRelevant;
    int PICAM_TimeStampBitDepthRelevant;
    int PICAM_TimeStampResolutionRelevant;
    int PICAM_TimeStampsRelevant;
    int PICAM_TrackFramesRelevant;
    int PICAM_CcdCharacteristicsRelevant;
    int PICAM_PixelGapHeightRelevant;
    int PICAM_PixelGapWidthRelevant;
    int PICAM_PixelHeightRelevant;
    int PICAM_PixelWidthRelevant;
    int PICAM_SensorActiveBottomMarginRelevant;
    int PICAM_SensorActiveHeightRelevant;
    int PICAM_SensorActiveLeftMarginRelevant;
    int PICAM_SensorActiveRightMarginRelevant;
    int PICAM_SensorActiveTopMarginRelevant;
    int PICAM_SensorActiveWidthRelevant;
    int PICAM_SensorMaskedBottomMarginRelevant;
    int PICAM_SensorMaskedHeightRelevant;
    int PICAM_SensorMaskedTopMarginRelevant;
    int PICAM_SensorSecondaryActiveHeightRelevant;
    int PICAM_SensorSecondaryMaskedHeightRelevant;
    int PICAM_SensorTypeRelevant;
    int PICAM_ActiveBottomMarginRelevant;
    int PICAM_ActiveHeightRelevant;
    int PICAM_ActiveLeftMarginRelevant;
    int PICAM_ActiveRightMarginRelevant;
    int PICAM_ActiveTopMarginRelevant;
    int PICAM_ActiveWidthRelevant;
    int PICAM_MaskedBottomMarginRelevant;
    int PICAM_MaskedHeightRelevant;
    int PICAM_MaskedTopMarginRelevant;
    int PICAM_SecondaryActiveHeightRelevant;
    int PICAM_SecondaryMaskedHeightRelevant;
    int PICAM_CleanBeforeExposureRelevant;
    int PICAM_CleanCycleCountRelevant;
    int PICAM_CleanCycleHeightRelevant;
    int PICAM_CleanSectionFinalHeightRelevant;
    int PICAM_CleanSectionFinalHeightCountRelevant;
    int PICAM_CleanSerialRegisterRelevant;
    int PICAM_CleanUntilTriggerRelevant;
    int PICAM_DisableCoolingFanRelevant;
    int PICAM_EnableSensorWindowHeaterRelevant;
    int PICAM_SensorTemperatureReadingRelevant;
    int PICAM_SensorTemperatureSetPointRelevant;
    int PICAM_SensorTemperatureStatusRelevant;
#define PICAM_LAST_PARAM PICAM_SensorTemperatureStatusRelevant

private:
    PicamHandle currentCameraHandle;
    int selectedCameraIndex;
	const PicamCameraID *availableCameraIDs;
	const PicamCameraID *unavailableCameraIDs;
    piint availableCamerasCount;
    piint unavailableCamerasCount;
    asynStatus initializeDetector();
    asynStatus piClearParameterRelevance(asynUser *pasynUser);
    asynStatus piLoadUnavailableCameraIDs();
    int piLookupDriverParameter(PicamParameter picamParameter);
    asynStatus piRegisterConstraintChangeWatch(PicamHandle cameraHandle);
    asynStatus piRegisterRelevantWatch(PicamHandle cameraHandle);
    asynStatus piRegisterValueChangeWatch(PicamHandle cameraHandle);
	asynStatus piSetParameterRelevance(asynUser *pasynUser,
        PicamParameter parameter,
		int relevence);
	asynStatus piSetParameterValuesFromSelectedCamera();
	asynStatus piSetRois(int minX, int minY,
			int width, int height,
			int binX, int binY);
	asynStatus piSetSelectedCamera(asynUser *pasynUser, int selectedIndex);
    asynStatus piSetSelectedUnavailableCamera(asynUser *pasynUser, 
        int selectedIndex);
    asynStatus piUnregisterConstraintChangeWatch(PicamHandle cameraHandle);
    asynStatus piUnregisterRelevantWatch(PicamHandle cameraHandle);
    asynStatus piUnregisterValueChangeWatch(PicamHandle cameraHandle);
    asynStatus piUpdateParameterRelevance(asynUser *pasynUser);
    asynStatus piUpdateAvailableCamerasList();
    asynStatus piUpdateUnavailableCamerasList();

    static ADPICam *ADPICam_Instance;
};

//_____________________________________________________________________________
#define PICAM_VersionNumberString          "PICAM_VERSION_NUMBER"
//Available Camera List
#define PICAM_AvailableCamerasString       "PICAM_AVAILABLE_CAMERAS"
#define PICAM_CameraInterfaceString        "PICAM_CAMERA_INTERFACE"
#define PICAM_SensorNameString             "PICAM_SENSOR_NAME"
#define PICAM_SerialNumberString           "PICAM_SERIAL_NUMBER"
#define PICAM_FirmwareRevisionString       "PICAM_FIRMWARE_REVISION"
//Unavailable Camera List
#define PICAM_UnavailableCamerasString          "PICAM_UNAVAILABLE_CAMERAS"
#define PICAM_CameraInterfaceUnavailableString  "PICAM_CAMERA_INTERFACE_UNAVAILABLE"
#define PICAM_SensorNameUnavailableString       "PICAM_SENSOR_NAME_UNAVAILABLE"
#define PICAM_SerialNumberUnavailableString     "PICAM_SERIAL_NUMBER_UNAVAILABLE"
#define PICAM_FirmwareRevisionUnavailableString "PICAM_FIRMWARE_REVISION_UNAVAILABLE"
//AnalogToDigitalConversion
#define PICAM_AdcAnalogGainString                      "PICAM_ADC_ANALOG_GAIN"
#define PICAM_AdcBitDepthString                        "PICAM_ADC_BIT_DEPTH"
#define PICAM_AdcEMGainString                          "PICAM_ADC_EM_GAIN"
#define PICAM_AdcQualityString                         "PICAM_ADC_QUALITY"
#define PICAM_AdcSpeedString                           "PICAM_ADC_SPEED"
#define PICAM_CorrectPixelBiasString                   "PICAM_CORRECT_PIXEL_BIAS"
//Readout Calculations
#define PICAM_ReadoutTimeCalcString                    "PICAM_READOUT_TIME_CALC"
#define PICAM_ReadoutRateCalcString                    "PICAM_READOUT_RATE_CALC"
#define PICAM_FrameRateCalcString                       "PICAM_FRAME_RATE_CALC"
#define PICAM_OnlineReadoutRateCalcString          "PICAM_ONLINE_READOUT_RATE_CALC"

// Camera Parameter Relevance
#define PICAM_ExposureTimeRelString           "PICAM_EXPOSURE_TIME_PR"
#define PICAM_ShutterClosingDelayRelString    "PICAM_SHUTTER_CLOSING_DELAY_PR"       
#define PICAM_ShutterDelayResolutionRelString "PICAM_SHUTTER_DELAY_RESOLUTION_PR"     
#define PICAM_ShutterOpeningDelayRelString    "PICAM_SHUTTER_OPEN_DELAY_PR" 
#define PICAM_ShutterTimingModeRelString      "PICAM_SHUTTER_TIMING_MODE_PR"
#define PICAM_BracketGatingRelString          "PICAM_BRACKET_GATING_PR"
#define PICAM_CustomModulationSequenceRelString      "PICAM_CUSTOM_MODULATION_SEQUENCE_PR"  
#define PICAM_DifEndingGateRelString                 "PICAM_DIF_END_GATE_PR"
#define PICAM_DifStartingGateRelString               "PICAM_DIF_START_GATE_PR"
#define PICAM_EMIccdGainRelString                    "PICAM_EMI_CCD_GAIN_PR"
#define PICAM_EMIccdGainControlMode                  "PICAM_EMI_CCD_GAIN_CTL_MODE_PR"
#define PICAM_EnableIntensifierRelString             "PICAM_ENABLE_INTENSIFIER_PR"  
#define PICAM_EnableModulationRelString              "PICAM_ENABLE_MODULATION_PR"
#define PICAM_GatingModeRelString                    "PICAM_GATING_MODE_PR"
#define PICAM_GatingSpeedRelString                   "PICAM_GATING_SPEED_PR"
#define PICAM_IntensifierDiameterRelString           "PICAM_INTENSIFIER_DIAM_PR"
#define PICAM_IntensifierGainRelString               "PICAM_INTENSIFIER_GAIN_PR"
#define PICAM_IntensifierOptionsRelString            "PICAM_INTENSIFIER_OPTIONS_PR"
#define PICAM_IntensifierStatusRelString             "PICAM_INTENSIFIER_STATUS_PR"
#define PICAM_ModulationDurationRelString            "PICAM_MODULATION_DURATION_PR"
#define PICAM_ModulationFrequencyRelString           "PICAM_MODULATION_FREQUENCY_PR"
#define PICAM_PhosphorDecayDelayRelString            "PICAM_PHOSFOR_DECAY_DELAY_PR"
#define PICAM_PhosphorDecayDelayResolutionRelString  "PICAM_PHOSFOR_DELAY_DECAY_RES_PR"  
#define PICAM_PhosphorTypeRelString                  "PICAM_PHOSFOR_TYPE_PR"
#define PICAM_PhotocathodeSensitivityRelString       "PICAM_PHOTOCATHODE_SENSITIVITY_PR"
#define PICAM_RepetitiveGateRelString                "PICAM_REPETITIVE_GATE_PR"
#define PICAM_RepetitiveModulationPhaseRelString     "PICAM_REPETITIVE_MODULATION_PR" 
#define PICAM_SequentialStartingModulationPhaseRelString "PICAM_SEQ_STARTING_MODULATION_PHASE_PR"
#define PICAM_SequentialEndingModulationPhaseRelString   "PICAM_SEQ_END_MODULATION_PHASE_PR"
#define PICAM_SequentialEndingGateRelString          "PICAM_SEQ_END_GATE_PR"
#define PICAM_SequentialGateStepCountRelString       "PICAM_SEQ_GATE_STEP_COUNT_PR"
#define PICAM_SequentialGateStepIterationsRelString  "PICAM_SEQ_GATE_STEP_ITERATIONS_PR"    
#define PICAM_SequentialStartingGateRelString        "PICAM_SEQ_START_GATE_PR" 
#define PICAM_AdcAnalogGainRelString                 "PICAM_ADC_ANALOG_GAIN_PR"
#define PICAM_AdcBitDepthRelString                   "PICAM_ADC_BIT_DEPTH_PR"
#define PICAM_AdcEMGainRelString                     "PICAM_ADC_EM_GAIN_PR"
#define PICAM_AdcQualityRelString                    "PICAM_ADC_QUALITY_PR"
#define PICAM_AdcSpeedRelString                      "PICAM_ADC_SPEED_PR"
#define PICAM_CorrectPixelBiasRelString              "PICAM_CORRECT_PIXEL_BIAS_PR" 
#define PICAM_AuxOutputRelString                     "PICAM_AUX_OUTPUT_PR"
#define PICAM_EnableModulationOutputSignalRelString          "PICAM_ENABLE_MODULATION_OUT_SIGNAL_PR"
#define PICAM_EnableModulationOutputSignalFrequencyRelString "PICAM_ENABLE_MODULATION_OUT_SIGNAL_FREQ_PR"
#define PICAM_EnableModulationOutputSignalAmplitudeRelString "PICAM_ENABLE_MODULATION_OUT_SIGNAL_AMPL_PR"
#define PICAM_EnableSyncMasterRelString              "PICAM_SYNC_MASTER_PR"
#define PICAM_InvertOutputSignalRelString            "PICAM_INVERT_OUTPUT_SIGNAL_PR" 
#define PICAM_OutputSignalRelString                  "PICAM_OUTPUT_SIGNAL_PR"
#define PICAM_SyncMaster2DelayRelString              "PICAM_SYNC_MASTER2_DELAY_PR"    
#define PICAM_TriggerCouplingRelString               "PICAM_TRIGGER_COUPLING_PR"
#define PICAM_TriggerDeterminationRelString          "PICAM_TRIGGER_DETERMINATION_PR" 
#define PICAM_TriggerFrequencyRelString              "PICAM_TRIGGER_FREQUENCY_PR"
#define PICAM_TriggerResponseRelString               "PICAM_TRIGGER_RESPONSE_PR"
#define PICAM_TriggerSourceRelString                 "PICAM_TRIGGER_SOURCE_PR"
#define PICAM_TriggerTerminationRelString            "PICAM_TRIGGER_TERMINATION_PR"
#define PICAM_TriggerThresholdRelString              "PICAM_TRIGGER_THRESHOLD_PR"
#define PICAM_AccumulationsRelString                 "PICAM_ACCUMULATIONS_PR"
#define PICAM_EnableNondestructiveReadoutRelString   "PICAM_ENABLE_NONDESTRUCT_READOUT_PR"   
#define PICAM_KineticsWindowHeightRelString          "PICAM_KINETICS_WINDOW_HEIGHT_PR"
#define PICAM_NondestructiveReadoutPeriodRelString   "PICAM_NONDESTRUCT_READOUT_PERIOD_PR"
#define PICAM_ReadoutControlModeRelString            "PICAM_READOUT_CONTROL_MODE_PR"
#define PICAM_ReadoutOrientationRelString            "PICAM_READOUNT_ORIENTATION_PR"
#define PICAM_ReadoutPortCountRelString              "PICAM_READOUT_PORT_COUNT_PR"
#define PICAM_ReadoutTimeCalculationRelString        "PICAM_READOUT_TIME_CALC_PR"
#define PICAM_VerticalShiftRateRelString             "PICAM_VERTICAL_SHIFT_RATE_PR"
#define PICAM_DisableDataFormattingRelString         "PICAM_DISABLE_DATA_FORMATTING_PR"
#define PICAM_ExactReadoutCountMaximumRelString      "PICAM_EXACT_READOUT_COUNT_MAX_PR"
#define PICAM_FrameRateCalculationRelString          "PICAM_FRAME_RATE_CALC_PR"
#define PICAM_FrameSizeRelString                     "PICAM_FRAME_SIZE_PR"
#define PICAM_FramesPerReadoutRelString              "PICAM_FRAMES_PER_READOUT_PR"
#define PICAM_FrameStrideRelString                   "PICAM_FRAME_STRIDE_PR"
#define PICAM_FrameTrackingBitDepthRelString         "PICAM_FRAME_TRK_BIT_DEPTH_PR"
#define PICAM_GateTrackingRelString                  "PICAM_GATE_TRACKING_PR"
#define PICAM_GateTrackingBitDepthRelString          "PICAM_FRAME_TRACKING_BIT_DEPTH_PR"
#define PICAM_ModulationTrackingRelString            "PICAM_MODULATION_TRACKING_PR"
#define PICAM_ModulationTrackingBitDepthRelString    "PICAM_MODULATION_TRACKING_BIT_DEPTH_PR"   
#define PICAM_NormalizeOrientationRelString          "PICAM_NORMALIZE_ORIENTATION_PR"
#define PICAM_OnlineReadoutRateCalculationRelString  "PICAM_ONLINE_READOUT_RATE_CALC_PR"
#define PICAM_OrientationRelString                   "PICAM_ORIENTATION_PR"
#define PICAM_PhotonDetectionModeRelString           "PICAM_PHOTON_DETECTION_MODE_PR" 
#define PICAM_PhotonDetectionThresholdRelString      "PICAM_PHOTON_DETECT_THRESHOLD_PR"
#define PICAM_PixelBitDepthRelString                 "PICAM_PIXEL_BIT_DEPTH_PR"
#define PICAM_PixelFormatRelString                   "PICAM_PIXEL_FORMAT_PR"
#define PICAM_ReadoutCountRelString                  "PICAM_READOUT_COUNT_PR"
#define PICAM_ReadoutRateCalculationRelString   "PICAM_READOUT_RATE_CALC_PR"  
#define PICAM_ReadoutStrideRelString                 "PICAM_READOUT_STRIDE_PR"
#define PICAM_RoisRelString                          "PICAM_ROIS_PR"
#define PICAM_TimeStampBitDepthRelString             "PICAM_TIME_STAMP_BIT_DEPTH_PR"
#define PICAM_TimeStampResolutionRelString           "PICAM_TIME_STAMP_RESOLUTION_PR"  
#define PICAM_TimeStampsRelString                    "PICAM_TIME_STAMPS_PR"
#define PICAM_TrackFramesRelString                   "PICAM_TRACK_FRAMES_PR"   
#define PICAM_CcdCharacteristicsRelString            "PICAM_CCD_CHARACTERISTICS_PR"
#define PICAM_PixelGapHeightRelString                "PICAM_PIXEL_GAP_HEIGHT_PR"
#define PICAM_PixelGapWidthRelString                 "PICAM_PIXEL_GAP_WIDTH_PR"
#define PICAM_PixelHeightRelString                   "PICAM_PIXEL_HEIGHT_PR"
#define PICAM_PixelWidthRelString                    "PICAM_PIXEL_WIDTH_PR"
#define PICAM_SensorActiveBottomMarginRelString      "PICAM_SENSOR_ACTIVE_BOTTOM_MARGIN_PR"
#define PICAM_SensorActiveHeightRelString            "PICAM_SENSOR_ACTIVE_HEIGHT_PR"
#define PICAM_SensorActiveLeftMarginRelString        "PICAM_SENSOR_ACTIVE_LEFT_MARGIN_PR"
#define PICAM_SensorActiveRightMarginRelString       "PICAM_SENSOR_ACTIVE_RIGHT_MARGIN_PR"
#define PICAM_SensorActiveTopMarginRelString         "PICAM_SENSOR_ACTIVE_TOP_MARGIN_PR"
#define PICAM_SensorActiveWidthRelString             "PICAM_SENSOR_ACTIVE_WIDTH_PR"
#define PICAM_SensorMaskedBottomMarginRelString      "PICAM_SENSOR_MASK_BOTTOM_MARGIN_PR"
#define PICAM_SensorMaskedHeightRelString            "PICAM_SENSOR_MASK_HEIGHT_PR"
#define PICAM_SensorMaskedTopMarginRelString         "PICAM_SENSOR_MASK_TOP_MARGIN_PR"
#define PICAM_SensorSecondaryActiveHeightRelString   "PICAM_SENSOR_SECONDARY_ACTIVE_HEIGHT_PR"  
#define PICAM_SensorSecondaryMaskedHeightRelString   "PICAM_SENSOR_SECONDARY_MASK_HEIGHT_PR"   
#define PICAM_SensorTypeRelString                   "PICAM_SENSOR_TYPE_PR"
#define PICAM_ActiveBottomMarginRelString            "PICAM_ACTIVE_BOTTOM_MARGIN_PR"   
#define PICAM_ActiveHeightRelString                  "PICAM_ACTIVE_HEIGHT_PR"
#define PICAM_ActiveLeftMarginRelString              "PICAM_ACTIVE_LEFT_MARGIN_PR"
#define PICAM_ActiveRightMarginRelString             "PICAM_ACTIVE_RIGHT_MARGIN_PR"
#define PICAM_ActiveTopMarginRelString               "PICAM_ACTIVE_TOP_MARGIN_PR"
#define PICAM_ActiveWidthRelString                   "PICAM_ACTIVE_WIDTH_PR"
#define PICAM_MaskedBottomMarginRelString            "PICAM_MASK_BOTTOM_MARGIN_PR"  
#define PICAM_MaskedHeightRelString                  "PICAM_MASK_HEIGHT_PR"
#define PICAM_MaskedTopMarginRelString               "PICAM_MASK_TOP_MARGIN_PR"
#define PICAM_SecondaryActiveHeightRelString         "PICAM_SECONDARY_ACTIVE_HEIGHT_PR"
#define PICAM_SecondaryMaskedHeightRelString         "PICAM_SECONDARY_MASK_HEIGHT_PR"
#define PICAM_CleanBeforeExposureRelString           "PICAM_CLEAN_BEFORE_EXPOSURE_PR"
#define PICAM_CleanCycleCountRelString               "PICAM_CLEAN_CYCLE_COUNT_PR"
#define PICAM_CleanCycleHeightRelString              "PICAM_CLEAN_CYCLE_HEIGHT_PR"
#define PICAM_CleanSectionFinalHeightRelString       "PICAM_CLEAN_SECTION_FINAL_HEIGHT_PR"      
#define PICAM_CleanSectionFinalHeightCountRelString  "PICAM_CLEAN_SECTION_FINAL_COUNT_PR"      
#define PICAM_CleanSerialRegisterRelString           "PICAM_CLEAN_SERIAL_REGISTER_PR"
#define PICAM_CleanUntilTriggerRelString             "PICAM_CLEAN_UNTIL_TRIG_PR"
#define PICAM_DisableCoolingFanRelString             "PICAM_DISABLE_COOLING_FAN_PR"
#define PICAM_EnableSensorWindowHeaterRelString      "PICAM_ENABLE_WINDOW_SENSOR_HEATER_PR"
#define PICAM_SensorTemperatureReadingRelString      "PICAM_SENSOR_TEMPERATURE_READING_PR"       
#define PICAM_SensorTemperatureSetPointRelString     "PICAM_SENSOR_TEMPERATURE_SETPOINT_PR"       
#define PICAM_SensorTemperatureStatusRelString       "PICAM_SENSOR_TEMPERATURE_STATUS_PR"      

#define NUM_PICAM_PARAMS ((int)(&PICAM_LAST_PARAM - &PICAM_FIRST_PARAM + 1))
//_____________________________________________________________________________

#endif