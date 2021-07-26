using System;
using System.Collections.Generic;

namespace TinyParserMitsuba
{
    public class SceneLoader
    {
        public SceneLoader()
        {
            _LookupPaths = new();
            _Arguments = new();
            _DisableLowerCaseConversion = false;
        }

        public Scene? LoadFromFile(string path)
        {
            tpm_load_options options = new();
            // TODO
            options.disable_lowercase_conversion = (byte)(_DisableLowerCaseConversion ? 1 : 0);
            var scene_handle = Lib.tpm_load_file2(path, ref options);
            if (scene_handle == IntPtr.Zero)
                return null;
            else
                return new Scene(scene_handle);
        }

        public Scene? LoadFromString(string str)
        {
            tpm_load_options options = new();
            // TODO
            options.disable_lowercase_conversion = (byte)(_DisableLowerCaseConversion ? 1 : 0);
            var scene_handle = Lib.tpm_load_string2(str, ref options);
            if (scene_handle == IntPtr.Zero)
                return null;
            else
                return new Scene(scene_handle);
        }

        void AddLookupDir(string path)
        {
            _LookupPaths.Add(path);
        }

        void AddArgument(string key, string value)
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

        private List<string> _LookupPaths;
        private Dictionary<string, string> _Arguments;
        private bool _DisableLowerCaseConversion;
    }
}