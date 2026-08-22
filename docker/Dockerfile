# Rakennetaan SDK-kuvassa ja ajetaan kevyemmassa ajoymparistossa.
# Build-konteksti on repon juuri, koska lahteet ovat DotNET/-kansiossa.
FROM mcr.microsoft.com/dotnet/sdk:10.0 AS build
WORKDIR /src

# Projektitiedostot ensin, jotta restore-kerros osuu valimuistiin kun vain koodi muuttuu.
COPY DotNET/CoreDto/CoreDto.csproj CoreDto/
COPY DotNET/KuivuriWeb/KuivuriWeb.csproj KuivuriWeb/
RUN dotnet restore KuivuriWeb/KuivuriWeb.csproj

COPY DotNET/CoreDto/ CoreDto/
COPY DotNET/KuivuriWeb/ KuivuriWeb/
RUN dotnet publish KuivuriWeb/KuivuriWeb.csproj -c Release -o /app --no-restore

FROM mcr.microsoft.com/dotnet/aspnet:10.0
WORKDIR /app
COPY --from=build /app .

# Kuvaajien kellonajat ovat paikallista aikaa (ToLocalTime), joten vyohyke on asetettava.
# tzdata sisaltyy aspnet-kuvaan, joten erillista asennusta ei tarvita.
ENV TZ=Europe/Helsinki
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

# Pohjaimage kuuntelee oletuksena porttia 8080 (ASPNETCORE_HTTP_PORTS).
EXPOSE 8080

ENTRYPOINT ["dotnet", "KuivuriWeb.dll"]
