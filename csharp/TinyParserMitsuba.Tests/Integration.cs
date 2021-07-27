using NUnit.Framework;

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
            Scene scene = loader.LoadFromString("");
            Assert.IsNull(scene);
        }
    }
}