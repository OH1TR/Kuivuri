using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using CoreDto;
using Dto;
using KuivuriWeb.DBContext;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Options;

namespace KuivuriWeb.Controllers
{
    [Route("api/[controller]")]
    [ApiController]
    public class KuivuriController : ControllerBase
    {
        private readonly IOptions<Config> config;

        public KuivuriController(IOptions<Config> config)
        {
            this.config = config;
        }

        [HttpPost]
        [Route("measureData")]
        public void MeasureData([FromBody] MeasureData value)
        {
            if (config.Value.ApiKey != value.ApiKey)
                return;

            KuivuriContext ctx = new KuivuriContext();
            var data = new Measurement() { MachineName = value.MachineName, Temp1 = value.Temp1, Temp2 = value.Temp2, Hairio = value.Hairio, Jaahdytys = value.Jaahdytys, Kuivaus = value.Kuivaus };
            ctx.Measurement.Add(data);
            ctx.SaveChanges();
        }

        [HttpGet]
        [Route("get12hData")]
        public Trace[] Get12hData()
        {
            KuivuriContext ctx = new KuivuriContext();
            DateTime now = DateTime.UtcNow.FloorSeconds();
            DateTime minus12 = DateTime.UtcNow.AddHours(-12);
            DateTime[] times = new DateTime[12 * 60];
            double[] values1 = new double[12 * 60];
            double[] values2 = new double[12 * 60];

            for (int i = 0; i < times.Length; i++)
                times[i] = minus12.AddMinutes(i).ToLocalTime();

            foreach (var m in ctx.Measurement.Where(i => i.Created > minus12).ToArray().GroupBy(t => new { t.Created }).Select(i => new { i.Key.Created, Temp1 = i.Average(p => p.Temp1), Temp2 = i.Average(p => p.Temp2) }))
            {
                int index = (int)((m.Created.FloorSeconds() - minus12).TotalMinutes);
                if (index > 0 && index < times.Length)
                {
                    values1[index] = m.Temp1;
                    values2[index] = m.Temp2;
                }
            }

            // Last slot is not always filled.
            if (values1[values1.Length - 1] == 0)
                values1[values1.Length - 1] = values1[values1.Length - 2];

            if (values2[values2.Length - 1] == 0)
                values2[values2.Length - 1] = values2[values2.Length - 2];

            return new Trace[] { new Trace() { x = times, y = values1 }, new Trace() { x = times, y = values2 } };
        }

        [HttpGet]
        [Route("getCurrent")]
        public CurrentValues GetCurrent()
        {
            KuivuriContext ctx = new KuivuriContext();
            var data = ctx.Measurement.OrderByDescending(i => i.Created).First();

            if (data != null)
                return new CurrentValues() { Time = data.Created.FloorSeconds().ToLocalTime(), Temp1 = data.Temp1, Temp2 = data.Temp2 };

            return null;
        }
    }
}
