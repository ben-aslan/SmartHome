// See https://aka.ms/new-console-template for more information
using System.IO.Ports;
using Telegram.Bot;

var tClient = new TelegramBotClient("1478694857:AAEgh3QRaiqk2vKHFowgUTYWphkSEImDqvY");

Console.WriteLine("Hello, World!");
SerialPort serialPort = new SerialPort(SerialPort.GetPortNames()[2], 9600);
serialPort.DataReceived += new System.IO.Ports.SerialDataReceivedEventHandler(DataReceivedHandler);


void DataReceivedHandler(object sender, SerialDataReceivedEventArgs e)
{
    try
    {
        SerialPort sp = (SerialPort)sender;
        string indata = sp.ReadExisting();
        tClient.SendMessage(1144576556, indata);
        //Do what ever you want with the data
        Thread.Sleep(10000);
    }
    catch (Exception)
    {

    }
}

try
{
    serialPort.Open();
}
catch (Exception)
{

}

Console.ReadKey();
