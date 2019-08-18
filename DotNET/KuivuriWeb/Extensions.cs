using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace KuivuriWeb
{
    public static class Extensions
    {
        public static DateTime FloorSeconds(this DateTime value)
        {
            return new DateTime(value.Year, value.Month, value.Day, value.Hour, value.Minute, 0);
        }

    }
}
