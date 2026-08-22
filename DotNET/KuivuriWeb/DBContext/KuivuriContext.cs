using Microsoft.EntityFrameworkCore;

namespace KuivuriWeb.DBContext
{
    public class KuivuriContext : DbContext
    {
        public KuivuriContext()
        {
        }

        public KuivuriContext(DbContextOptions<KuivuriContext> options) : base(options)
        {
        }

        public DbSet<Measurement> Measurement { get; set; }

        protected override void OnConfiguring(DbContextOptionsBuilder optionsBuilder)
        {
            if (!optionsBuilder.IsConfigured)
                optionsBuilder.UseSqlite("Data Source=Kuivuri.db");
        }
    }
}
