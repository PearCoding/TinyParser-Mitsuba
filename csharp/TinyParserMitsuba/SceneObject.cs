using System;
using System.Collections.Generic;
using System.Text;

namespace TinyParserMitsuba
{
    public class SceneObject
    {
        internal SceneObject(IntPtr handle)
        {
            _Handle = handle;

            int prop_count = (int)Lib.tpm_get_property_count(_Handle);
            Properties = new(prop_count);
            for (int i = 0; i < prop_count; ++i)
            {
                int size = (int)Lib.tpm_get_property_key(_Handle, (ulong)i, null);
                if (size > 0)
                {
                    StringBuilder sb = new(size);
                    Lib.tpm_get_property_key(_Handle, (ulong)i, sb);
                    string name = sb.ToString();
                    Properties.Add(name, new(Lib.tpm_get_property(_Handle, name)));
                }
            }

            int named_count = (int)Lib.tpm_get_named_child_count(_Handle);
            NamedChildren = new(named_count);
            for (int i = 0; i < named_count; ++i)
            {
                int size = (int)Lib.tpm_get_named_child_key(_Handle, (ulong)i, null);
                if (size > 0)
                {
                    StringBuilder sb = new(size);
                    Lib.tpm_get_named_child_key(_Handle, (ulong)i, sb);
                    string name = sb.ToString();
                    NamedChildren.Add(name, new(Lib.tpm_get_named_child(_Handle, name)));
                }
            }

            int anony_count = (int)Lib.tpm_get_anonymous_child_count(_Handle);
            AnonymousChildren = new(anony_count);
            for (int i = 0; i < anony_count; ++i)
                AnonymousChildren.Add(new(Lib.tpm_get_anonymous_child(_Handle, (ulong)i)));
        }

        ~SceneObject()
        {
            Lib.tpm_free_object(_Handle);
        }

        public Dictionary<string, Property> Properties { get; }
        public Dictionary<string, SceneObject> NamedChildren { get; }
        public List<SceneObject> AnonymousChildren { get; }

        public ObjectType Type { get => (ObjectType)Lib.tpm_get_object_type(_Handle); }
        public string PluginType
        {
            get
            {
                int size = (int)Lib.tpm_get_plugin_type(_Handle, null);
                if (size > 0)
                {
                    StringBuilder sb = new(size);
                    Lib.tpm_get_plugin_type(_Handle, sb);
                    return sb.ToString();
                }
                else
                {
                    return "";
                }
            }
        }


        private IntPtr _Handle;
    }
}