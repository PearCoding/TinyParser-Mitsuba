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

        int MajorVersion { get; }
        int MinorVersion { get; }
        int PatchVersion { get; }
    }
}