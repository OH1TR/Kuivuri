using System;

namespace CoreDto
{
    public class Trace
    {
        public DateTime[] x { get; set; }
        public double[] y { get; set; }
        public string type { get; set; } = "scatter";
        public string yaxis { get; set; } = "y1";
    }
}
