using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;

namespace TinyParserMitsuba
{
    public class SceneLoader
    {
        public SceneLoader()
        {
            _LookupPaths = new();
            _Arguments = new();
            _DisableLowerCaseConversion = false;
            Error = "";
        }

        internal class OptionWrapper
        {
            public tpm_load_options Options;
            public List<IntPtr> ArgumentKeys;
            public List<IntPtr> ArgumentValues;
            public List<IntPtr> LookupDirs;

            public OptionWrapper()
            {
                Options = new();
                ArgumentKeys = new();
                ArgumentValues = new();
                LookupDirs = new();
            }

            public void Inject()
            {
                for (int i = 0; i < ArgumentKeys.Count; ++i)
                {
                    Marshal.WriteIntPtr(Options.argument_keys, i * Marshal.SizeOf<IntPtr>(), ArgumentKeys[i]);
                }
                for (int i = 0; i < ArgumentValues.Count; ++i)
                {
                    Marshal.WriteIntPtr(Options.argument_values, i * Marshal.SizeOf<IntPtr>(), ArgumentValues[i]);
                }
                for (int i = 0; i < LookupDirs.Count; ++i)
                {
                    Marshal.WriteIntPtr(Options.lookup_dirs, i * Marshal.SizeOf<IntPtr>(), LookupDirs[i]);
                }
            }

            public void Free()
            {
                if (Options.argument_keys != IntPtr.Zero)
                    Marshal.FreeHGlobal(Options.argument_keys);
                if (Options.argument_values != IntPtr.Zero)
                    Marshal.FreeHGlobal(Options.argument_values);
                if (Options.lookup_dirs != IntPtr.Zero)
                    Marshal.FreeHGlobal(Options.lookup_dirs);

                foreach (IntPtr ptr in ArgumentKeys)
                    Marshal.FreeHGlobal(ptr);
                foreach (IntPtr ptr in ArgumentValues)
                    Marshal.FreeHGlobal(ptr);
                foreach (IntPtr ptr in LookupDirs)
                    Marshal.FreeHGlobal(ptr);
            }
        }
        private OptionWrapper GetOptions()
        {
            OptionWrapper wrapper = new();
            wrapper.Options.argument_counts = (ulong)_Arguments.Count;
            wrapper.Options.argument_keys = _Arguments.Count == 0 ? IntPtr.Zero : Marshal.AllocHGlobal(Marshal.SizeOf<IntPtr>() * _Arguments.Count);
            wrapper.Options.argument_values = _Arguments.Count == 0 ? IntPtr.Zero : Marshal.AllocHGlobal(Marshal.SizeOf<IntPtr>() * _Arguments.Count);
            foreach (var x in _Arguments)
            {
                wrapper.ArgumentKeys.Add(Marshal.StringToHGlobalAnsi(x.Key));
                wrapper.ArgumentValues.Add(Marshal.StringToHGlobalAnsi(x.Value));
            }

            wrapper.Options.lookup_dirs_count = (ulong)_LookupPaths.Count;
            wrapper.Options.lookup_dirs = _LookupPaths.Count == 0 ? IntPtr.Zero : Marshal.AllocHGlobal(Marshal.SizeOf<IntPtr>() * _LookupPaths.Count);
            for (int i = 0; i < _LookupPaths.Count; ++i)
            {
                wrapper.LookupDirs.Add(Marshal.StringToHGlobalAnsi(_LookupPaths[i]));
            }

            wrapper.Options.disable_lowercase_conversion = (byte)(_DisableLowerCaseConversion ? 1 : 0);
            wrapper.Options.error_callback = new(str => Error = str);

            wrapper.Inject();
            return wrapper;
        }

        public Scene? LoadFromFile(string path)
        {
            Error = "";
            var wrapper = GetOptions();
            var options = wrapper.Options;
            var scene_handle = Lib.tpm_load_file2(path, ref options);
            wrapper.Free();

            if (scene_handle == IntPtr.Zero)
                return null;
            else
                return new Scene(scene_handle);
        }

        public Scene? LoadFromString(string str)
        {
            Error = "";
            var wrapper = GetOptions();
            var options = wrapper.Options;
            var scene_handle = Lib.tpm_load_string2(str, ref options);
            wrapper.Free();

            if (scene_handle == IntPtr.Zero)
                return null;
            else
                return new Scene(scene_handle);
        }

        public void AddLookupDir(string path)
        {
            _LookupPaths.Add(path);
        }

        public void AddArgument(string key, string value)
        {
            _Arguments.Add(key, value);
        }

        public List<string> LookupPaths
        {
            get => _LookupPaths;
        }

        public Dictionary<string, string> Arguments
        {
            get => _Arguments;
        }

        public bool DisableLowerCaseConversion
        {
            get => _DisableLowerCaseConversion;
            set => _DisableLowerCaseConversion = value;
        }

        public string Error { get; set; }
        public bool HasError { get => Error.Length > 0; }

        private List<string> _LookupPaths;
        private Dictionary<string, string> _Arguments;
        private bool _DisableLowerCaseConversion;
    }
}