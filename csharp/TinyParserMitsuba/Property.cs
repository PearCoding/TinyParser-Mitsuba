using System;

namespace TinyParserMitsuba
{
    public class Property
    {
        internal Property(IntPtr handle)
        {
            _Handle = handle;
        }

        ~Property()
        {
            Lib.tpm_free_property(_Handle);
        }

        public PropertyType Type { get => (PropertyType)Lib.tpm_get_property_type(_Handle); }

        public long GetInteger(long def = 0)
        {
            byte ok = 0;
            return Lib.tpm_property_get_int2(_Handle, def, ref ok);
        }

        public long? GetIntegerUnsafe(long def = 0)
        {
            byte ok = 0;
            var v = Lib.tpm_property_get_int2(_Handle, def, ref ok);
            return ok != 0 ? v : null;
        }

        public double GetNumber(double def = 0)
        {
            byte ok = 0;
            return Lib.tpm_property_get_number2(_Handle, def, ref ok);
        }

        public double? GetNumberUnsafe(double def = 0)
        {
            byte ok = 0;
            var v = Lib.tpm_property_get_number2(_Handle, def, ref ok);
            return ok != 0 ? v : null;
        }

        public bool GetBool(bool def = false)
        {
            byte ok = 0;
            return Lib.tpm_property_get_bool2(_Handle, (byte)(def ? 1 : 0), ref ok) != 0 ? true : false;
        }

        public bool? GetBoolUnsafe(bool def = false)
        {
            byte ok = 0;
            var v = Lib.tpm_property_get_bool2(_Handle, (byte)(def ? 1 : 0), ref ok) != 0 ? true : false;
            return ok != 0 ? v : null;
        }

        // Vector
        public double[] GetVector()
        {
            return GetVector(new double[3] { 0, 0, 0 });
        }

        public double[] GetVector(double[] def)
        {
            byte ok = 0;
            var input = new tpm_triplet { e1 = def[0], e2 = def[1], e3 = def[2] };
            var triplet = Lib.tpm_property_get_vector2(_Handle, ref input, ref ok);
            return new double[3] { triplet.e1, triplet.e2, triplet.e3 };
        }

        public double[] GetVectorUnsafe(double[] def)
        {
            byte ok = 0;
            var input = new tpm_triplet { e1 = def[0], e2 = def[1], e3 = def[2] };
            var triplet = Lib.tpm_property_get_vector2(_Handle, ref input, ref ok);
            return ok != 0 ? new double[3] { triplet.e1, triplet.e2, triplet.e3 } : null;
        }

        // Transform
        public double[] GetTransform()
        {
            byte ok = 0;
            var v = Lib.tpm_property_get_transform(_Handle, ref ok);
            return v.elements;
        }

        public double[] GetTransform(double[] def)
        {
            byte ok = 0;
            var input = new tpm_transform { elements = def };
            var v = Lib.tpm_property_get_transform2(_Handle, ref input, ref ok);
            return v.elements;
        }

        public double[] GetTransformUnsafe(double[] def)
        {
            byte ok = 0;
            var input = new tpm_transform { elements = def };
            var v = Lib.tpm_property_get_transform2(_Handle, ref input, ref ok);
            return ok != 0 ? v.elements : null;
        }

        // Color
        public double[] GetColor()
        {
            return GetColor(new double[3] { 0, 0, 0 });
        }

        public double[] GetColor(double[] def)
        {
            byte ok = 0;
            var input = new tpm_triplet { e1 = def[0], e2 = def[1], e3 = def[2] };
            var triplet = Lib.tpm_property_get_color2(_Handle, ref input, ref ok);
            return new double[3] { triplet.e1, triplet.e2, triplet.e3 };
        }

        public double[] GetColorUnsafe(double[] def)
        {
            byte ok = 0;
            var input = new tpm_triplet { e1 = def[0], e2 = def[1], e3 = def[2] };
            var triplet = Lib.tpm_property_get_color2(_Handle, ref input, ref ok);
            return ok != 0 ? new double[3] { triplet.e1, triplet.e2, triplet.e3 } : null;
        }

        // String

        public string GetString(string def = "")
        {
            byte ok = 0;
            var v = Lib.tpm_property_get_string2(_Handle, def, ref ok);
            return v;
        }

        public string GetStringUnsafe(string def = "")
        {
            byte ok = 0;
            var v = Lib.tpm_property_get_string2(_Handle, def, ref ok);
            return ok != 0 ? v : null;
        }

        // Spectrum
        public Spectrum GetSpectrum()
        {
            byte ok = 0;
            var v = Lib.tpm_property_get_spectrum(_Handle, ref ok);
            return ok != 0 ? new Spectrum(v) : null;
        }

        public Spectrum GetSpectrumUnsafe()
        {
            return GetSpectrum();
        }

        // Blackbody
        public Blackbody GetBlackbody()
        {
            byte ok = 0;
            var v = Lib.tpm_property_get_blackbody(_Handle, ref ok);
            return new Blackbody { Temperature = v.temperature, Scale = v.scale };
        }

        public Blackbody GetBlackbody(Blackbody def)
        {
            byte ok = 0;
            var input = new tpm_blackbody { temperature = def.Temperature, scale = def.Scale };
            var v = Lib.tpm_property_get_blackbody2(_Handle, ref input, ref ok);
            return new Blackbody { Temperature = v.temperature, Scale = v.scale };
        }

        public Blackbody? GetBlackbodyUnsafe(Blackbody def)
        {
            byte ok = 0;
            var input = new tpm_blackbody { temperature = def.Temperature, scale = def.Scale };
            var v = Lib.tpm_property_get_blackbody2(_Handle, ref input, ref ok);
            return ok != 0 ? new Blackbody { Temperature = v.temperature, Scale = v.scale } : null;
        }

        private IntPtr _Handle;
    }
}