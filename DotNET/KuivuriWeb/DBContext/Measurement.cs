using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.Linq;
using System.Threading.Tasks;

namespace KuivuriWeb.DBContext
{
    public class Measurement
    {
        [Key]
        public string Id { get; set; } = Guid.NewGuid().ToString();
        public string MachineName { get; set; }
        public double Temp1 { get; set; }
        public double Temp2 { get; set; }
        public bool Kuivaus { get; set; }
        public bool Jaahdytys { get; set; }
        public bool Hairio { get; set; }
        public DateTime Created { get; set; } = DateTime.UtcNow;
    }
}
