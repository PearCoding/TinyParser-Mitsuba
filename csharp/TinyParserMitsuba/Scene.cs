using System;

namespace TinyParserMitsuba
{
    public class Scene : SceneObject
    {
        internal Scene(IntPtr handle) : base(handle)
        {
            var version = Lib.tpm_get_scene_version(handle);
            MajorVersion = version.major;
            MinorVersion = version.minor;
            PatchVersion = version.patch;
        }

        public int MajorVersion { get; }
        public int MinorVersion { get; }
        public int PatchVersion { get; }
    }
}