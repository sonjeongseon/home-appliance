using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO.Ports;

namespace airconditioner
{
    public partial class Form1 : Form
    {
        private SerialPort serialPort1 = new SerialPort();

        public Form1()
        {
            InitializeComponent();
            serialPort1.PortName = "COM3";
            serialPort1.BaudRate = 115200;
            serialPort1.Open();

        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }

        private void button1_Click(object sender, EventArgs e)
        {
            serialPort1.Write("1");
            if (!serialPort1.IsOpen) return;
        }

        private void button2_Click(object sender, EventArgs e)
        {
            serialPort1.Write("2");
            if (!serialPort1.IsOpen) return;
        }

        private void button3_Click(object sender, EventArgs e)
        {
            serialPort1.Write("3");
            if (!serialPort1.IsOpen) return;
        }

        private void button4_Click(object sender, EventArgs e)
        {
            serialPort1.Write("4");
            if (!serialPort1.IsOpen) return;
        }

        private void button5_Click(object sender, EventArgs e)
        {
            serialPort1.Write("5");
            if (!serialPort1.IsOpen) return;
        }


    }
}
