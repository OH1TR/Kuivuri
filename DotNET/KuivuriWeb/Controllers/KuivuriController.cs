using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Dto;
using Microsoft.AspNetCore.Mvc;

namespace KuivuriWeb.Controllers
{
    [Route("api/[controller]")]
    [ApiController]
    public class KuivuriController : ControllerBase
    {
        [HttpPost]
        [Route("measureData")]
        public void MeasureData([FromBody] MeasureData value)
        {
        }
    }
}
