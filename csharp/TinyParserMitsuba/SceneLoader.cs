using System;
using System.Collections.Generic;
using System.Linq;

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

        private tpm_load_options GetOptions()
        {
            tpm_load_options options = new();
            options.argument_counts = (ulong)_Arguments.Count;
            options.argument_keys = _Arguments.Count == 0 ? null : new string[_Arguments.Count];
            options.argument_values = _Arguments.Count == 0 ? null : new string[_Arguments.Count];
            foreach (var x in _Arguments.Select((Entry, Index) => new { Entry, Index }))
            {
                options.argument_keys[x.Index] = x.Entry.Key;
                options.argument_values[x.Index] = x.Entry.Value;
            }

            options.lookup_dirs_count = (ulong)_LookupPaths.Count;
            options.lookup_dirs = _LookupPaths.Count == 0 ? null : new string[_LookupPaths.Count];
            for (int i = 0; i < _LookupPaths.Count; ++i)
            {
                options.lookup_dirs[i] = _LookupPaths[i];
            }

            options.disable_lowercase_conversion = (byte)(_DisableLowerCaseConversion ? 1 : 0);

            options.error_callback = new(str => Error = str);
            return options;
        }

        public Scene? LoadFromFile(string path)
        {
            Error = "";
            var options = GetOptions();
            var scene_handle = Lib.tpm_load_file2(path, ref options);
            if (scene_handle == IntPtr.Zero)
                return null;
            else
                return new Scene(scene_handle);
        }

        public Scene? LoadFromString(string str)
        {
            Error = "";
            var options = GetOptions();
            var scene_handle = Lib.tpm_load_string2(str, ref options);
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