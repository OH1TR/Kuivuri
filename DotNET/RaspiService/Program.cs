using Dto;
using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Diagnostics;
using System.IO;
using System.IO.Ports;
using System.Net;
using System.Text;

namespace RaspiService
{
    class Program
    {
        static void Main(string[] args)
        {
            System.Net.ServicePointManager.ServerCertificateValidationCallback += (o, certificate, chain, errors) => true;

            SerialPort mySerialPort = new SerialPort("/dev/ttyS0");

            mySerialPort.BaudRate = 9600;
            mySerialPort.Parity = Parity.None;
            mySerialPort.StopBits = StopBits.One;
            mySerialPort.DataBits = 8;
            mySerialPort.Handshake = Handshake.None;

            //mySerialPort.DataReceived += new SerialDataReceivedEventHandler(DataReceivedHandler);

            mySerialPort.Open();

            DateTime lastRead = DateTime.Now;

            while (true)
            {
                System.Threading.Thread.Sleep(100);

                try
                {
                    if (mySerialPort.BytesToRead > 0)
                        DataReceivedHandler(mySerialPort, null);

                    lock (dataBuffer)
                    {
                        for (int i = 0; i < bufferPos; i++)
                        {
                            if (dataBuffer[i] == 10)
                            {
                                ProcessLine(ExtractLine(i));
                                break;
                            }
                        }
                    }


                    if ((DateTime.Now - lastRead).TotalSeconds > 10)
                    {
                        Console.WriteLine("Sending:DS1820 READ SENSORS");
                        mySerialPort.Write("DS1820 READ SENSORS\r");
                        lastRead = DateTime.Now;
                    }
                }
                catch (Exception e)
                {
                    Console.WriteLine(e.Message + e.StackTrace);
                }

            }

            //mySerialPort.Close();
        }

        static private byte[] dataBuffer = new byte[1000];
        static private int bufferPos = 0;
        private static void DataReceivedHandler(object sender, SerialDataReceivedEventArgs e)
        {
            lock (dataBuffer)
            {
                SerialPort sp = (SerialPort)sender;

                int c = sp.Read(dataBuffer, bufferPos, sp.BytesToRead);
                bufferPos += c;
            }
        }

        static private string ExtractLine(int pos)
        {
            string line = "";

            for (int i = 0; i < pos; i++)
            {
                if (dataBuffer[i] == 13 || dataBuffer[i] == 10)
                    continue;

                line += System.Text.Encoding.ASCII.GetString(new byte[] { dataBuffer[i] });
            }

            for (int i = pos + 1; i < bufferPos; i++)
                dataBuffer[i - pos] = dataBuffer[i];

            bufferPos -= pos + 1;

            return (line);
        }

        //OK:10767DB50108009D:0023:0009:103B8BB501080098:003B:000C:FFFFFFFFFFFFFFFF:0000::        
        //DDDNo sensors
        //Error

        static private void ProcessLine(string line)
        {
            Console.WriteLine("Processing line:" + line);

            if (line.StartsWith("OK:"))
            {
                string[] toks = line.Split(":".ToCharArray());
                if (toks.Length == 11)
                {
                    SendData(HexToDouble(toks[2], toks[3]), HexToDouble(toks[5], toks[6]));
                }
            }
        }

        static private double HexToDouble(string cel, string celFrag)
        {
            try
            {
                int iCel = int.Parse(cel, System.Globalization.NumberStyles.HexNumber);
                int iCelFrag = int.Parse(celFrag, System.Globalization.NumberStyles.HexNumber);

                double retval = iCel;
                retval += (double)iCelFrag * 1 / 16;
                return (retval);
            }
            catch (Exception ex)
            {
                return (0);
            }
        }

        public static void SendData(double value1, double value2)
        {
            var baseAddress = "http://192.168.98.51:60017/api/kuivuri/measureData";

            var http = (HttpWebRequest)WebRequest.Create(new Uri(baseAddress));
            http.Accept = "application/json";
            http.ContentType = "application/json";
            http.Method = "POST";


            var md = new MeasureData();
            md.MachineName = Environment.MachineName;
            md.Temp1 = value1;
            md.Temp2 = value2;
            md.Kuivaus = false;
            md.Jaahdytys = false;
            md.Hairio = false;

            string parsedContent = JsonConvert.SerializeObject(md);

            ASCIIEncoding encoding = new ASCIIEncoding();
            Byte[] bytes = encoding.GetBytes(parsedContent);

            Stream newStream = http.GetRequestStream();
            newStream.Write(bytes, 0, bytes.Length);
            newStream.Close();

            var response = http.GetResponse();

            var stream = response.GetResponseStream();
            var sr = new StreamReader(stream);
            var content = sr.ReadToEnd();
        }
    }
}
