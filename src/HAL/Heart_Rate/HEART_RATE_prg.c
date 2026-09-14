#include "../../LIB/STD_TYPES.h"

#include "HEART_RATE_cfg.h"
#include "HEART_RATE_int.h"

typedef struct
{
    s32 BaselineSum;
    u16 BaselineBuffer[HHEART_RATE_BASELINE_WINDOW];
    u8 BaselineIndex;
    u8 BaselineFilled;
    s32 RecentMax;
    s32 RecentMin;
    s32 FilteredAc;
    u8 ExtremaInitialized;
    u8 AboveThreshold;
    u32 MsSinceLastBeat;
    u32 BeatIntervals[HHEART_RATE_HISTORY_LEN];
    u8 IntervalIndex;
    u8 IntervalsFilled;
    u8 ConsecutiveRejects;
    u8 ValidBeatCount;
    u8 SignalStableSamples;
    u16 Bpm;
    u8 NewBpm;
} HeartRate_State_t;

static HeartRate_State_t G_xHeartRate;

void HHeartRate_vInit(void)
{
    G_xHeartRate = (HeartRate_State_t){0};
}

void HHeartRate_vProcessSample(u16 A_u16AdcSample)
{
    s32 L_s32Baseline;
    s32 L_s32AcRaw;
    s32 L_s32AcValue;
    s32 L_s32PeakToPeak;
    s32 L_s32Threshold;

    G_xHeartRate.BaselineSum -= G_xHeartRate.BaselineBuffer[G_xHeartRate.BaselineIndex];
    G_xHeartRate.BaselineBuffer[G_xHeartRate.BaselineIndex] = A_u16AdcSample;
    G_xHeartRate.BaselineSum += A_u16AdcSample;
    G_xHeartRate.BaselineIndex =
        (G_xHeartRate.BaselineIndex + 1U) % HHEART_RATE_BASELINE_WINDOW;

    if (G_xHeartRate.BaselineFilled < HHEART_RATE_BASELINE_WINDOW)
    {
        G_xHeartRate.BaselineFilled++;
        G_xHeartRate.MsSinceLastBeat += HHEART_RATE_SAMPLE_PERIOD_MS;
        return;
    }

    L_s32Baseline = G_xHeartRate.BaselineSum / (s32)HHEART_RATE_BASELINE_WINDOW;
    L_s32AcRaw = (s32)A_u16AdcSample - L_s32Baseline;
    G_xHeartRate.FilteredAc +=
        ((L_s32AcRaw - G_xHeartRate.FilteredAc) * (s32)HHEART_RATE_LPF_ALPHA_NUM) /
        (s32)HHEART_RATE_LPF_ALPHA_DEN;
    L_s32AcValue = G_xHeartRate.FilteredAc;

    if (G_xHeartRate.ExtremaInitialized == 0U)
    {
        G_xHeartRate.RecentMax = L_s32AcValue;
        G_xHeartRate.RecentMin = L_s32AcValue;
        G_xHeartRate.ExtremaInitialized = 1U;
    }

    if (L_s32AcValue > G_xHeartRate.RecentMax) G_xHeartRate.RecentMax = L_s32AcValue;
    if (L_s32AcValue < G_xHeartRate.RecentMin) G_xHeartRate.RecentMin = L_s32AcValue;

    G_xHeartRate.RecentMax -= G_xHeartRate.RecentMax / 200;
    G_xHeartRate.RecentMin -= G_xHeartRate.RecentMin / 200;

    L_s32PeakToPeak = G_xHeartRate.RecentMax - G_xHeartRate.RecentMin;
    L_s32Threshold = G_xHeartRate.RecentMin + (L_s32PeakToPeak / 2);
    G_xHeartRate.MsSinceLastBeat += HHEART_RATE_SAMPLE_PERIOD_MS;

    if (L_s32PeakToPeak < HHEART_RATE_MIN_SIGNAL_AMPLITUDE)
    {
        G_xHeartRate.AboveThreshold = 0U;
        G_xHeartRate.ValidBeatCount = 0U;
        G_xHeartRate.IntervalsFilled = 0U;
        G_xHeartRate.IntervalIndex = 0U;
        G_xHeartRate.SignalStableSamples = 0U;
        return;
    }

    if (G_xHeartRate.SignalStableSamples < HHEART_RATE_STABLE_SAMPLES)
    {
        G_xHeartRate.SignalStableSamples++;
        return;
    }

    if ((L_s32AcValue > L_s32Threshold) &&
        (G_xHeartRate.AboveThreshold == 0U) &&
        (G_xHeartRate.MsSinceLastBeat >= HHEART_RATE_REFRACTORY_MS))
    {
        u32 L_u32CandidateInterval = G_xHeartRate.MsSinceLastBeat;

        G_xHeartRate.AboveThreshold = 1U;
        if ((L_u32CandidateInterval <
             (60000UL / HHEART_RATE_MAX_VALID_BPM)) ||
            (L_u32CandidateInterval >
             (60000UL / HHEART_RATE_MIN_VALID_BPM)))
        {
            G_xHeartRate.MsSinceLastBeat = 0U;
            return;
        }

        if (G_xHeartRate.IntervalsFilled > 0U)
        {
            u32 L_u32Sum = 0U;
            u32 L_u32Average;
            u32 L_u32Difference;

            for (u8 i = 0U; i < G_xHeartRate.IntervalsFilled; i++)
            {
                L_u32Sum += G_xHeartRate.BeatIntervals[i];
            }
            L_u32Average = L_u32Sum / G_xHeartRate.IntervalsFilled;
            L_u32Difference = (L_u32Average > L_u32CandidateInterval)
                ? (L_u32Average - L_u32CandidateInterval)
                : (L_u32CandidateInterval - L_u32Average);

            if ((L_u32Difference * HHEART_RATE_OUTLIER_RATIO_DEN >
                 L_u32Average * HHEART_RATE_OUTLIER_RATIO_NUM) &&
                (G_xHeartRate.ConsecutiveRejects < HHEART_RATE_OUTLIER_RESET_COUNT))
            {
                G_xHeartRate.ConsecutiveRejects++;
                G_xHeartRate.MsSinceLastBeat = 0U;
                return;
            }
        }

        G_xHeartRate.ConsecutiveRejects = 0U;
        G_xHeartRate.ValidBeatCount++;
        G_xHeartRate.BeatIntervals[G_xHeartRate.IntervalIndex] = L_u32CandidateInterval;
        G_xHeartRate.IntervalIndex =
            (G_xHeartRate.IntervalIndex + 1U) % HHEART_RATE_HISTORY_LEN;
        if (G_xHeartRate.IntervalsFilled < HHEART_RATE_HISTORY_LEN)
        {
            G_xHeartRate.IntervalsFilled++;
        }

        if (G_xHeartRate.ValidBeatCount >= HHEART_RATE_MIN_VALID_BEATS)
        {
            u32 L_u32Sum = 0U;
            for (u8 i = 0U; i < G_xHeartRate.IntervalsFilled; i++)
            {
                L_u32Sum += G_xHeartRate.BeatIntervals[i];
            }
            G_xHeartRate.Bpm =
                (u16)(60000UL / (L_u32Sum / G_xHeartRate.IntervalsFilled));
            G_xHeartRate.NewBpm = 1U;
        }

        G_xHeartRate.MsSinceLastBeat = 0U;
    }
    else if (L_s32AcValue < (L_s32Threshold - (L_s32PeakToPeak / 4)))
    {
        G_xHeartRate.AboveThreshold = 0U;
    }
}

u8 HHeartRate_u8GetBpm(u16* A_pu16Bpm)
{
    if (A_pu16Bpm == NULL)
    {
        return 0U;
    }

    if (G_xHeartRate.NewBpm == 0U)
    {
        return 0U;
    }

    *A_pu16Bpm = G_xHeartRate.Bpm;
    G_xHeartRate.NewBpm = 0U;
    return 1U;
}