using System;
using System.Collections.Generic;

namespace TinyParserMitsuba
{
    public class Spectrum
    {
        internal Spectrum(IntPtr handle)
        {
            IsUniform = Lib.tpm_spectrum_is_uniform(handle) != 0;

            int count = (int)Lib.tpm_spectrum_get_length(handle);
            Wavelengths = new(count);
            Weights = new(count);

            for (int i = 0; i < count; ++i)
            {
                Wavelengths.Add(Lib.tpm_spectrum_get_wavelength(handle, (ulong)i));
                Weights.Add(Lib.tpm_spectrum_get_weight(handle, (ulong)i));
            }

            Lib.tpm_free_spectrum(handle);
        }

        public List<double> Wavelengths { get; }
        public List<double> Weights { get; }
        public bool IsUniform { get; }
    }
}