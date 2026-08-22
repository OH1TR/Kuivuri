using System;

namespace Dto
{
    public class MeasureData
    {
        public string MachineName { get; set; }
        public double Temp1 { get; set; }
        public double Temp2 { get; set; }
        public bool Kuivaus { get; set; }
        public bool Jaahdytys { get; set; }
        public bool Hairio { get; set; }
        public string ApiKey { get; set; }
    }
}
