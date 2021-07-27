namespace TinyParserMitsuba
{
    public enum ObjectType
    {
        Scene = 0,
        Bsdf,
        Emitter,
        Film,
        Integrator,
        Medium,
        Phase,
        RFilter,
        Sampler,
        Sensor,
        Shape,
        Subsurface,
        Texture,
        Volume
    }

    public enum PropertyType
    {
        None = 0,
        Animation,
        Blackbody,
        Bool,
        Integer,
        Number,
        Color,
        Spectrum,
        String,
        Transform,
        Vector
    }
}