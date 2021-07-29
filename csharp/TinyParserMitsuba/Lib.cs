using System;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Text;

#pragma warning disable IDE1006 // Naming convention might be weird here, but thats on purpose
namespace TinyParserMitsuba
{
    #region Types
    using tpm_object_handle = IntPtr;
    using tpm_property_handle = IntPtr;
    using tpm_spectrum_handle = IntPtr;
    using tpm_size = UInt64;
    using tpm_int64 = Int64;
    using tpm_bool = Byte;
    #endregion

    #region Structures
    internal enum tpm_object_type
    {
        TPM_OT_ERROR = -1,
        TPM_OT_SCENE = 0,
        TPM_OT_BSDF,
        TPM_OT_EMITTER,
        TPM_OT_FILM,
        TPM_OT_INTEGRATOR,
        TPM_OT_MEDIUM,
        TPM_OT_PHASE,
        TPM_OT_RFILTER,
        TPM_OT_SAMPLER,
        TPM_OT_SENSOR,
        TPM_OT_SHAPE,
        TPM_OT_SUBSURFACE,
        TPM_OT_TEXTURE,
        TPM_OT_VOLUME
    }

    internal enum tpm_property_type
    {
        TPM_PT_ERROR = -1,
        TPM_PT_NONE = 0,
        TPM_PT_ANIMATION,
        TPM_PT_BLACKBODY,
        TPM_PT_BOOL,
        TPM_PT_INTEGER,
        TPM_PT_NUMBER,
        TPM_PT_COLOR,
        TPM_PT_SPECTRUM,
        TPM_PT_STRING,
        TPM_PT_TRANSFORM,
        TPM_PT_VECTOR,
    }

    delegate void tpm_error_callback([MarshalAs(UnmanagedType.LPStr)] string str);
    [StructLayout(LayoutKind.Sequential)]
    internal struct tpm_load_options
    {
        public IntPtr lookup_dirs;
        public tpm_size lookup_dirs_count;
        public IntPtr argument_keys;
        public IntPtr argument_values;
        public tpm_size argument_counts;
        public tpm_bool disable_lowercase_conversion;
        public tpm_error_callback error_callback;
    }

    [StructLayout(LayoutKind.Sequential)]
    internal struct tpm_version
    {
        public int major;
        public int minor;
        public int patch;
    }

    [StructLayout(LayoutKind.Sequential)]
    internal struct tpm_triplet
    {
        public double e1;
        public double e2;
        public double e3;
    }

    [StructLayout(LayoutKind.Sequential)]
    internal struct tpm_transform
    {
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 16)]
        public double[] elements;
    }

    [StructLayout(LayoutKind.Sequential)]
    internal struct tpm_blackbody
    {
        public double temperature;
        public double scale;
    }
    #endregion

    internal static class Lib
    {
        internal const string LibName = "tinyparser_mitsuba_c";

        static Lib()
        {
            NativeLibrary.SetDllImportResolver(typeof(Lib).Assembly, ImportResolver);
        }

        private static IntPtr ImportResolver(string libraryName, Assembly assembly, DllImportSearchPath? dllImportSearchPath)
        {
            string mappedName = libraryName;

            // Linking on OS X only works correctly if the file contains the version number.
            if (libraryName == LibName && RuntimeInformation.IsOSPlatform(OSPlatform.OSX))
            {
                mappedName = "libtinyparser_mitsuba_c.1.dylib";
            }

            return NativeLibrary.Load(mappedName, assembly, dllImportSearchPath);
        }

        #region Core
        [DllImport(LibName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void tpm_free_object(tpm_object_handle handle);
        [DllImport(LibName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void tpm_free_property(tpm_property_handle handle);
        [DllImport(LibName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void tpm_free_spectrum(tpm_spectrum_handle handle);
        [DllImport(LibName, CallingConvention = CallingConvention.Cdecl)]
        public static extern tpm_object_type tpm_get_object_type(tpm_object_handle handle);
        [DllImport(LibName, CallingConvention = CallingConvention.Cdecl)]
        public static extern tpm_property_type tpm_get_property_type(tpm_property_handle handle);
        #endregion

        #region Loader
        [DllImport(LibName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern tpm_object_handle tpm_load_file(string path);
        [DllImport(LibName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern tpm_object_handle tpm_load_file2(string path, ref tpm_load_options options);

        [DllImport(LibName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern tpm_object_handle tpm_load_string(string str);
        [DllImport(LibName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern tpm_object_handle tpm_load_string2(string str, ref tpm_load_options options);

        [DllImport(LibName, CallingConvention = CallingConvention.Cdecl)]
        public static extern tpm_object_handle tpm_load_memory(byte[] data, tpm_size size);
        [DllImport(LibName, CallingConvention = CallingConvention.Cdecl)]
        public static extern tpm_object_handle tpm_load_memory2(byte[] data, tpm_size size, ref tpm_load_options options);
        #endregion

        #region Version
        [DllImport(LibName, CallingConvention = CallingConvention.Cdecl)]
        public static extern tpm_version tpm_get_version();
        [DllImport(LibName, CallingConvention = CallingConvention.Cdecl)]
        public static extern tpm_version tpm_get_scene_version(tpm_object_handle handle);
        #endregion

        #region Object
        [DllImport(LibName, CallingConvention = CallingConvention.Cdecl)]
        public static extern tpm_bool tpm_has_plugin_type(tpm_object_handle handle);
        [DllImport(LibName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern tpm_size tpm_get_plugin_type(tpm_object_handle handle, StringBuilder str);
        [DllImport(LibName, CallingConvention = CallingConvention.Cdecl)]
        public static extern tpm_bool tpm_has_id(tpm_object_handle handle);
        [DllImport(LibName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern tpm_size tpm_get_id(tpm_object_handle handle, StringBuilder str);
        [DllImport(LibName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern tpm_property_handle tpm_get_property(tpm_object_handle handle, string key);
        [DllImport(LibName, CallingConvention = CallingConvention.Cdecl)]
        public static extern tpm_size tpm_get_property_count(tpm_object_handle handle);
        [DllImport(LibName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern tpm_size tpm_get_property_key(tpm_object_handle handle, tpm_size id, StringBuilder str);
        [DllImport(LibName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern tpm_object_handle tpm_get_named_child(tpm_object_handle handle, string key);
        [DllImport(LibName, CallingConvention = CallingConvention.Cdecl)]
        public static extern tpm_size tpm_get_named_child_count(tpm_object_handle handle);
        [DllImport(LibName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern tpm_size tpm_get_named_child_key(tpm_object_handle handle, tpm_size id, StringBuilder str);
        [DllImport(LibName, CallingConvention = CallingConvention.Cdecl)]
        public static extern tpm_object_handle tpm_get_anonymous_child(tpm_object_handle handle, tpm_size id);
        [DllImport(LibName, CallingConvention = CallingConvention.Cdecl)]
        public static extern tpm_size tpm_get_anonymous_child_count(tpm_object_handle handle);
        #endregion

        #region Property
        [DllImport(LibName, CallingConvention = CallingConvention.Cdecl)]
        public static extern tpm_bool tpm_property_get_bool(tpm_property_handle handle, ref tpm_bool ok);
        [DllImport(LibName, CallingConvention = CallingConvention.Cdecl)]
        public static extern tpm_bool tpm_property_get_bool2(tpm_property_handle handle, tpm_bool def, ref tpm_bool ok);
        [DllImport(LibName, CallingConvention = CallingConvention.Cdecl)]
        public static extern tpm_int64 tpm_property_get_int(tpm_property_handle handle, ref tpm_bool ok);
        [DllImport(LibName, CallingConvention = CallingConvention.Cdecl)]
        public static extern tpm_int64 tpm_property_get_int2(tpm_property_handle handle, tpm_int64 def, ref tpm_bool ok);
        [DllImport(LibName, CallingConvention = CallingConvention.Cdecl)]
        public static extern double tpm_property_get_number(tpm_property_handle handle, ref tpm_bool ok);
        [DllImport(LibName, CallingConvention = CallingConvention.Cdecl)]
        public static extern double tpm_property_get_number2(tpm_property_handle handle, double def, ref tpm_bool ok);

        [DllImport(LibName, CallingConvention = CallingConvention.Cdecl)]
        public static extern tpm_triplet tpm_property_get_vector(tpm_property_handle handle, ref tpm_bool ok);
        [DllImport(LibName, CallingConvention = CallingConvention.Cdecl)]
        public static extern tpm_triplet tpm_property_get_vector2(tpm_property_handle handle, ref tpm_triplet def, ref tpm_bool ok);
        [DllImport(LibName, CallingConvention = CallingConvention.Cdecl)]
        public static extern tpm_triplet tpm_property_get_color(tpm_property_handle handle, ref tpm_bool ok);
        [DllImport(LibName, CallingConvention = CallingConvention.Cdecl)]
        public static extern tpm_triplet tpm_property_get_color2(tpm_property_handle handle, ref tpm_triplet def, ref tpm_bool ok);

        [DllImport(LibName, CallingConvention = CallingConvention.Cdecl)]
        public static extern tpm_transform tpm_property_get_transform(tpm_property_handle handle, ref tpm_bool ok);
        [DllImport(LibName, CallingConvention = CallingConvention.Cdecl)]
        public static extern tpm_transform tpm_property_get_transform2(tpm_property_handle handle, ref tpm_transform def, ref tpm_bool ok);

        [DllImport(LibName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern tpm_size tpm_property_get_string(tpm_property_handle handle, ref tpm_bool ok, StringBuilder str);
        [DllImport(LibName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern tpm_size tpm_property_get_string2(tpm_property_handle handle, string def, ref tpm_bool ok, StringBuilder str);

        [DllImport(LibName, CallingConvention = CallingConvention.Cdecl)]
        public static extern tpm_blackbody tpm_property_get_blackbody(tpm_property_handle handle, ref tpm_bool ok);
        [DllImport(LibName, CallingConvention = CallingConvention.Cdecl)]
        public static extern tpm_blackbody tpm_property_get_blackbody2(tpm_property_handle handle, ref tpm_blackbody def, ref tpm_bool ok);

        [DllImport(LibName, CallingConvention = CallingConvention.Cdecl)]
        public static extern tpm_spectrum_handle tpm_property_get_spectrum(tpm_property_handle handle, ref tpm_bool ok);
        #endregion

        #region Spectrum
        [DllImport(LibName, CallingConvention = CallingConvention.Cdecl)]
        public static extern tpm_bool tpm_spectrum_is_uniform(tpm_spectrum_handle handle);
        [DllImport(LibName, CallingConvention = CallingConvention.Cdecl)]
        public static extern tpm_size tpm_spectrum_get_length(tpm_spectrum_handle handle);
        [DllImport(LibName, CallingConvention = CallingConvention.Cdecl)]
        public static extern double tpm_spectrum_get_wavelength(tpm_spectrum_handle handle, UInt64 index);
        [DllImport(LibName, CallingConvention = CallingConvention.Cdecl)]
        public static extern double tpm_spectrum_get_weight(tpm_spectrum_handle handle, UInt64 index);
        #endregion
    }
}