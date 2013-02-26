/** \file hwrng_limits.h
  *
  * \brief Statistical limits on what is considered a working HWRNG.
  *
  * The code in hwrng.c does statistical testing of samples from the
  * hardware random number generator (HWRNG). These constants define the limits
  * for each statistical test. These values are very dependent on the
  * implementation of the HWRNG.
  *
  * This file is licensed as described by the file LICENCE.
  */

#ifndef PIC32_HWRNG_LIMITS_H_INCLUDED
#define PIC32_HWRNG_LIMITS_H_INCLUDED

/** Nominal mean, in ADC output number. This is purely theoretical; the mean
  * should be set by an equal-resistor divider. */
#define STATTEST_CENTRAL_MEAN		511.5
/** Minimum acceptable mean, in ADC output number.
  * This differs from STATTEST_CENTRAL_MEAN by the following:
  * - Factor of 0.901: worst case decrease due to 2 5% tolerance resistors
  *   with a 45 K temperature change at 100 ppm/K.
  * - Offset of 65: worst case decrease due to 6 millivolt op-amp input offset
  *   voltage multiplied by a gain of 40.
  * - Offset of 8: maximum total absolute error of ADC.
  */
#define STATTEST_MIN_MEAN			(0.901 * STATTEST_CENTRAL_MEAN - 75.0 - 8.0)
/** Maximum acceptable mean, in ADC output number.
  * This differs from STATTEST_CENTRAL_MEAN by the following:
  * - Factor of 1.109: worst case increase due to 2 5% tolerance resistors
  *   with a 45 K temperature change at 100 ppm/K.
  * - Offset of 65: worst case increase due to 6 millivolt op-amp input offset
  *   voltage multiplied by a gain of 40.
  * - Offset of 8: maximum total absolute error of ADC.
  */
#define STATTEST_MAX_MEAN			(1.109 * STATTEST_CENTRAL_MEAN + 75.0 + 8.0)
/** Nominal variance, in ADC output number squared. This was measured. */
#define STATTEST_CENTRAL_VARIANCE	1402.3
/** Minimum acceptable variance, in ADC output number squared.
  * This differs from #STATTEST_CENTRAL_VARIANCE by the following factors:
  * - Factor of 0.846: variation in amplitude of Johnson-Nyquist noise due to
  *   temperature decrease from 293 K to 248 K.
  * - Factor of 0.656: worst case decrease due to 3 1% tolerance resistors
  *   and 4 5% tolerance resistors with a 45 K temperature change at 100 ppm/K.
  * - Factor of 0.798: 8 sigma statistical fluctuations for N = 4096. This was
  *   measured.
  * - Factor of 0.709: worst case decrease due to RC low-pass filter
  *   inaccuracy (5% tolerance resistor, 10% tolerance capacitor, 0.9%
  *   resistance increase over temperature, 15% capacitance increase over
  *   temperature).
  */
#define STATTEST_MIN_VARIANCE		(0.846 * 0.656 * 0.798 * 0.709 * STATTEST_CENTRAL_VARIANCE)
/** Maximum acceptable variance, in ADC output number squared.
  * This differs from #STATTEST_CENTRAL_VARIANCE by the following factors:
  * - Factor of 1.154: variation in amplitude of Johnson-Nyquist noise due to
  *   temperature increase from 293 K to 338 K.
  * - Factor of 1.523: worst case increase due to 3 1% tolerance resistors
  *   and 4 5% tolerance resistors with a 45 K temperature change at 100 ppm/K.
  * - Factor of 1.253: 8 sigma statistical fluctuations for N = 4096. This was
  *   measured.
  * - Factor of 1.409: worst case increase due to RC low-pass filter
  *   inaccuracy (5% tolerance resistor, 10% tolerance capacitor, 0.9%
  *   resistance decrease over temperature, 15% capacitance decrease over
  *   temperature).
  * - Factor of 2.5: allowable additive interference. Additive interference
  *   is undesirable, but it does not compromise the amount of entropy per
  *   sample because addition is a reversible operation. Limiting the allowable
  *   additive interference is necessary to prevent saturation (which is not
  *   a reversible operation and hence reduces the amount of entropy per
  *   sample) and arithmetic overflow.
  */
#define STATTEST_MAX_VARIANCE		(1.154 * 1.523 * 1.253 * 1.409 * 2.5 * STATTEST_CENTRAL_VARIANCE)
/** Maximum acceptable skewness (standardised 3rd central moment) in either
  * the positive or negative direction. This is approximately 10 standard
  * deviations from the theoretical value of 0.
  * This was measured.
  */
#define STATTEST_MAX_SKEWNESS		0.416
/** Minimum acceptable kurtosis (standardised 4th central moment - 3). This is
  * approximately 10 standard deviations below the
  * theoretical value of 0. This was measured.
  */
#define STATTEST_MIN_KURTOSIS		-0.83
/** Maximum acceptable kurtosis (standardised 4th central moment - 3). This is
  * approximately 5 standard deviations above the
  * theoretical value of 0. This was measured.
  * Note that even for N = 4096, the skewness of kurtosis distribution is
  * significant (about 0.35); that's why this is not just the negation
  * of #STATTEST_MIN_KURTOSIS.
  */
#define STATTEST_MAX_KURTOSIS		1.13
/** The bandwidth of the HWRNG is defined as the frequency range over which
  * the power spectral density remains higher than this threshold, relative
  * to the peak value. Conventionally, this would be 0.5, corresponding to
  * 3 dB. However, because statistics.c calculates a power spectral density
  * estimate, this must be lower than 0.5 to account for statistical
  * fluctuations.
  *
  * Like #PSD_THRESHOLD_REPETITIONS, this is a value which needs to be
  * determined empirically (in other words, tweak it until you get sensible
  * results). Too high a value will cause the bandwidth to be
  * underestimated, too low a value will cause overestimation. As some
  * guidance, for N = 4096, each bin in the PSD has a standard deviation of
  * about 1.7 dB (this was measured), so accounting for 5 sigma fluctuations
  * of a single bin means lowering the 3 dB threshold by about 8.5 dB.
  *
  * This is set to an absurdly low value because the BitSafe development board
  * thermal noise source is somewhat susceptible to capacitively coupled
  * interference. What this means is that if the board is placed near a human
  * (eg. in someone's hand), large peaks can appear in the power spectrum.
  * Despite this, the noise source is still an acceptable source of
  * entropy. Even an absurdly low value is still capable of detecting some
  * hardware failure modes.
  */
#define PSD_BANDWIDTH_THRESHOLD		0.03
/** Number of consecutive power spectrum bins which must be below the
  * threshold (see #PSD_BANDWIDTH_THRESHOLD) before the code in statistics.c
  * considers a bin as an edge of the HWRNG bandwidth. Making this value
  * larger has the effect of reducing the impact of statistical fluctuations.
  *
  * Like #PSD_BANDWIDTH_THRESHOLD, this is a value which needs to be
  * determined empirically (in other words, tweak it until you get sensible
  * results). As some guidance, to have a one in a million
  * chance of a falsely registered edge, the threshold must be lowered
  * by approximately inverf(1 - 1 / (500000 ^ (1 / (this)))) * sqrt(2)
  * standard deviations.
  */
#define PSD_THRESHOLD_REPETITIONS	5
/** The minimum acceptable value for the peak frequency in the power spectrum.
  * The value is expressed as a fraction of the sampling rate.
  * This value corresponds to about 500 Hz and was chosen because it is well
  * below the HWRNG filter's high-pass cutoff.
  */
#define PSD_MIN_PEAK				0.0208
/** The maximum acceptable value for the peak frequency in the power spectrum.
  * The value is expressed as a fraction of the sampling rate.
  * This value corresponds to about 9 kHz and was chosen because it is well
  * above the HWRNG filter's low-pass cutoff.
  */
#define PSD_MAX_PEAK				0.375
/** The minimum acceptable value for the bandwidth of the HWRNG.
  * The value is expressed as a fraction of the sampling rate.
  * Note that this should not be lowered to account for statistical
  * fluctuations, as they're should be taken care of
  * in the values of #PSD_BANDWIDTH_THRESHOLD and #PSD_THRESHOLD_REPETITIONS.
  *
  * The measured 3 dB bandwidth of the current HWRNG is about 4.5 kHz.
  */
#define PSD_MIN_BANDWIDTH			0.1875
/** The lag, in samples, to start applying the autocorrelation threshold
  * (see #AUTOCORR_THRESHOLD) to.
  *
  * For an ideal white noise source, this should be 1, so that every point
  * (excluding the first, corresponding to lag 0, which will trivially be a
  * large positive value) in the correlogram will be considered. However, in
  * reality, filtering of the HWRNG signal will cause low-lag parts of
  * the correlogram to divert away from 0 significantly. Those parts should be
  * ignored, as they are artefacts of filtering and not genuine indicators
  * of HWRNG failure.
  *
  * This value was estimated from an ensemble of measured correlograms.
  */
#define AUTOCORR_START_LAG			7
/** The normalised autocorrelation threshold. If the magnitude of any values
  * in the correlogram exceed this threshold, then the HWRNG is considered
  * to possess too much autocorrelation (i.e. it is not random).
  *
  * This is "normalised" in the following sense: the actual threshold is
  * this value, multiplied by the variance. This is done because
  * autocorrelation amplitude scales linearly with signal variance.
  *
  * This value was estimated by increasing it until the autocorrelation test
  * failed (due to capacitively coupled interference) at about the same time
  * as the peak detection test.
  */
#define AUTOCORR_THRESHOLD			2.8
/** Minimum acceptable entropy estimate (in bits) per sample. This is
  * approximately 8 standard deviations (calculated using N = 4096) below
  * the mean entropy estimate for a Gaussian distribution with a standard
  * deviation of 20. This was calculated using Monte Carlo simulation.
  */
#define STATTEST_MIN_ENTROPY		6.21

#endif // #ifndef PIC32_HWRNG_LIMITS_H_INCLUDED
