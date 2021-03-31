using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Micro2_Serial
{
    public partial class Form1 : Form
    {
        #region global_variables
        private string readBuffer = string.Empty;
        int x = 0;
        #endregion

        public Form1()
        {
            InitializeComponent();
        }

        #region connect_open_and_set_parameters_and_run_timer
        private void button1_Click(object sender, EventArgs e)
        {
            if (!serialPort1.IsOpen)
            {
                serialPort1.PortName = "COM5";
                serialPort1.Open();
                richTextBox1.Text = "COMPort opened!";
                serialPort1.ReceivedBytesThreshold = 20;
                serialPort1.NewLine = "\r";
                serialPort1.DataReceived += new System.IO.Ports.SerialDataReceivedEventHandler(SerialPort1_DataReceived);
                
            }
        }
        #endregion

        #region read_databuffer_received
        private void SerialPort1_DataReceived(System.Object sender, System.IO.Ports.SerialDataReceivedEventArgs e)
        {
            readBuffer = serialPort1.ReadLine();
            Invoke(new EventHandler(DoUpdate));
        }
        #endregion

        #region update_text
        public void DoUpdate(object sender, System.EventArgs e)
        {
            richTextBox1.Text = readBuffer;
        }
        #endregion

        #region close_button
        private void button2_Click(object sender, EventArgs e)
        {
            {
                serialPort1.Close();
                richTextBox1.Text = "Serial Port Closed!";
                MessageBox.Show("Simple Voltmeter Richard Ilot");
                this.Close();
            }
            #endregion
        }

/*        private void richTextBox1_TextChanged(object sender, EventArgs e)
        {

        }
*/
        private void Voltmeter_Load(object sender, EventArgs e)
        {

        }

        private void richTextBox1_TextChanged(object sender, EventArgs e)
        {

        }
    }
}
