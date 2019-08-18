using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using KuivuriWeb.DBContext;
using Microsoft.AspNetCore;
using Microsoft.AspNetCore.Hosting;
using Microsoft.EntityFrameworkCore.Infrastructure;
using Microsoft.EntityFrameworkCore.Storage;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.Logging;

namespace KuivuriWeb
{
    public class Program
    {
        public static void Main(string[] args)
        {
            if (!File.Exists("Kuivuri.db"))
            {
                using (var db = new KuivuriContext())
                {
                    RelationalDatabaseCreator databaseCreator =
                        (RelationalDatabaseCreator)db.Database.GetService<IDatabaseCreator>();
                    databaseCreator.CreateTables();
                }
            }

            CreateWebHostBuilder(args).Build().Run();
        }

        public static IWebHostBuilder CreateWebHostBuilder(string[] args) =>
            WebHost.CreateDefaultBuilder(args)
                    .ConfigureLogging((hostingContext, logging) =>
                    {
                        logging.AddConsole();
                        logging.AddDebug();
                    })
                .UseStartup<Startup>();
    }
}
