using NUnit.Framework;
using System.IO;

namespace TinyParserMitsuba.Tests
{
    public class Tests
    {
        [SetUp]
        public void Setup()
        {
        }

        [Test]
        public void TestEmpty()
        {
            SceneLoader loader = new();
            loader.AddArgument("test", "test");
            loader.AddLookupDir("test/");

            Scene scene = loader.LoadFromString("");
            Assert.IsNull(scene);
            Assert.IsTrue(loader.HasError);
        }

        [Test]
        public void TestSimple()
        {
            SceneLoader loader = new();
            Scene scene = loader.LoadFromString(@"
<?xml version='1.0' encoding='utf-8'?>
<scene version='0.5.0'>
    <integrator type='path'>
    </integrator>
</scene>
");
            Assert.IsNotNull(scene);
            Assert.IsFalse(loader.HasError);
            Assert.AreEqual(scene.Type, ObjectType.Scene);
            Assert.AreEqual(scene.MajorVersion, 0);
            Assert.AreEqual(scene.MinorVersion, 5);
            Assert.AreEqual(scene.PatchVersion, 0);
        }

        [Test]
        public void TestComplex()
        {
            SceneLoader loader = new();
            Scene scene = loader.LoadFromString(@"
<?xml version='1.0' encoding='utf-8'?>
<scene version='0.5.0'>
    <integrator type='path'>
		<integer name='maxDepth' value='65' />
        <boolean name='strictNormals' value='true' />
    </integrator>
    <sensor type='perspective' >
		<float name='fov' value='35' />
		<transform name='toWorld' >
			<matrix value='-0.999971 0.000344842 -0.00764602 0.0324209 -2.91047e-011 0.998984 0.045055 1.52673 0.00765379 0.0450537 -0.998955 4.88197 0 0 0 1'/>
		</transform>
		<sampler type='sobol' >
			<integer name='sampleCount' value='64' />
		</sampler>
		<film type='ldrfilm' >
			<integer name='width' value='720' />
			<integer name='height' value='1280' />
			<string name='fileFormat' value='png' />
			<string name='pixelFormat' value='rgb' />
			<float name='gamma' value='2.2' />
			<boolean name='banner' value='false' />
			<rfilter type='tent' />
		</film>
	</sensor>
    <texture id='tex' type='bitmap' >
		<string name='filename' value='textures/image.jpg' />
		<string name='filterType' value='trilinear' />
	</texture>
	<bsdf type='twosided' id='material' >
		<bsdf type='roughplastic' >
			<float name='alpha' value='0.1' />
			<string name='distribution' value='ggx' />
			<float name='intIOR' value='1.5' />
			<float name='extIOR' value='1' />
			<boolean name='nonlinear' value='true' />
			<ref id='tex' name='diffuseReflectance' />
		</bsdf>
	</bsdf>
    <shape type='sphere' >
		<float name='radius' value='2' />
		<transform name='toWorld' >
			<matrix value='1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1'/>
		</transform>
		<boolean name='faceNormals' value='true' />
		<ref id='material' />
	</shape>
    <shape type='rectangle' >
		<transform name='toWorld' >
			<matrix value='-1.03225 -6.81307e-015 1.55865e-007 -0.481175 -1.55865e-007 4.51209e-008 -1.03225 7.14059 0 -1.08037 -4.72247e-008 -1.65275 0 0 0 1'/>
		</transform>
		<bsdf type='twosided' >
			<bsdf type='diffuse' >
				<rgb name='reflectance' value='0, 0, 0'/>
			</bsdf>
		</bsdf>
		<emitter type='area' >
			<rgb name='radiance' value='30, 30, 30'/>
		</emitter>
	</shape>
</scene>
");
            Assert.IsNotNull(scene);
            Assert.IsFalse(loader.HasError);
            Assert.AreEqual(scene.Type, ObjectType.Scene);
            Assert.AreEqual(scene.MajorVersion, 0);
            Assert.AreEqual(scene.MinorVersion, 5);
            Assert.AreEqual(scene.PatchVersion, 0);
            Assert.AreEqual(scene.AnonymousChildren.Count, 6);

            Assert.AreEqual(scene.AnonymousChildren[0].Type, ObjectType.Integrator);
            Assert.AreEqual(scene.AnonymousChildren[1].Type, ObjectType.Sensor);
            Assert.AreEqual(scene.AnonymousChildren[2].Type, ObjectType.Texture);
            Assert.AreEqual(scene.AnonymousChildren[3].Type, ObjectType.Bsdf);
            Assert.AreEqual(scene.AnonymousChildren[4].Type, ObjectType.Shape);
            Assert.AreEqual(scene.AnonymousChildren[5].Type, ObjectType.Shape);

            var integrator = scene.AnonymousChildren[0];
            Assert.IsTrue(integrator.Properties.ContainsKey("max_depth"));
            Assert.IsTrue(integrator.Properties.ContainsKey("strict_normals"));

            var sensor = scene.AnonymousChildren[1];
            Assert.AreEqual(sensor.PluginType, "perspective");
            Assert.IsTrue(sensor.Properties.ContainsKey("fov"));
            Assert.IsTrue(sensor.Properties.ContainsKey("to_world"));
            Assert.AreEqual(sensor.AnonymousChildren.Count, 2);
            Assert.AreEqual(sensor.AnonymousChildren[0].Type, ObjectType.Sampler);
            Assert.AreEqual(sensor.AnonymousChildren[1].Type, ObjectType.Film);

            var tex = scene.AnonymousChildren[2];
            Assert.IsTrue(tex.Properties.ContainsKey("filename")); // Not loaded, as described in the README.md
            Assert.IsTrue(tex.Properties.ContainsKey("filter_type"));

            var bsdf = scene.AnonymousChildren[3];
            Assert.AreEqual(bsdf.AnonymousChildren.Count, 1);

            var innerbsdf = bsdf.AnonymousChildren[0];
            Assert.AreEqual(innerbsdf.AnonymousChildren.Count, 0);
            Assert.AreEqual(innerbsdf.Properties.Count, 5);
            Assert.AreEqual(innerbsdf.NamedChildren.Count, 1);
            Assert.IsTrue(innerbsdf.NamedChildren.ContainsKey("diffuse_reflectance"));

            var shape1 = scene.AnonymousChildren[4];
            Assert.IsTrue(shape1.Properties.ContainsKey("radius"));
            Assert.IsTrue(shape1.Properties.ContainsKey("to_world"));
            Assert.IsTrue(shape1.Properties.ContainsKey("face_normals"));
            Assert.AreEqual(shape1.AnonymousChildren.Count, 1); // Bsdf reference

            var shape2 = scene.AnonymousChildren[5];
            Assert.IsTrue(shape2.Properties.ContainsKey("to_world"));
            Assert.AreEqual(shape2.AnonymousChildren.Count, 2);
        }

        [Test]
        public void TestFile()
        {
            File.WriteAllText(TestContext.CurrentContext.WorkDirectory + "/test.xml",
                @"<?xml version='1.0' encoding='utf-8'?>
<scene version='0.5.0'>
    <integrator type='path'>
        <string name='param1' value='$test' />
    </integrator>
</scene>
");

            SceneLoader loader = new();
            loader.AddArgument("test", "test");
            loader.AddLookupDir(TestContext.CurrentContext.WorkDirectory);
            Scene scene = loader.LoadFromFile(TestContext.CurrentContext.WorkDirectory + "/test.xml");
            Assert.IsNotNull(scene);
            Assert.IsFalse(loader.HasError);
            Assert.AreEqual(scene.Type, ObjectType.Scene);
            Assert.AreEqual(scene.MajorVersion, 0);
            Assert.AreEqual(scene.MinorVersion, 5);
            Assert.AreEqual(scene.PatchVersion, 0);

        }
    }
}