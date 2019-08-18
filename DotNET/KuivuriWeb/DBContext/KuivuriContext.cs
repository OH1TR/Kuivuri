using Microsoft.EntityFrameworkCore;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace KuivuriWeb.DBContext
{
    public class KuivuriContext : DbContext
    {
        public DbSet<Measurement> Measurement { get; set; }

        protected override void OnConfiguring(DbContextOptionsBuilder optionsBuilder)
        {
            optionsBuilder.UseSqlite("Data Source=Kuivuri.db");
        }
    }
}
