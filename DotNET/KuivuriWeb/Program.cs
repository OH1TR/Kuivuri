using KuivuriWeb;
using KuivuriWeb.DBContext;
using Microsoft.AspNetCore.Builder;
using Microsoft.EntityFrameworkCore;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;

var builder = WebApplication.CreateBuilder(args);

builder.Services.AddControllers();
builder.Services.Configure<Config>(builder.Configuration.GetSection("Config"));
builder.Services.AddDbContext<KuivuriContext>(options =>
    options.UseSqlite(builder.Configuration.GetConnectionString("Kuivuri") ?? "Data Source=Kuivuri.db"));

var app = builder.Build();

using (var scope = app.Services.CreateScope())
{
    scope.ServiceProvider.GetRequiredService<KuivuriContext>().Database.EnsureCreated();
}

if (app.Environment.IsDevelopment())
    app.UseDeveloperExceptionPage();

app.UseFileServer();
app.MapControllers();

app.Run();
