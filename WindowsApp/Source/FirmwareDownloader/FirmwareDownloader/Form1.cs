using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using IniParser;
using IniParser.Model;

namespace FirmwareDownloader
{
    public partial class Form1 : Form
    {
        struct SerialNumber
        {
            public UInt64 start;
            public UInt64 max;
            public Byte length;
            public UInt64 address;
        }
        SerialNumber serial_number;
        const string CONFIGURATION_FILE = "settings.ini";
        const string JLINK_TEMPLATE_FILE = "script.jlink";
        const string JLINK_SCRIPT_FILE = "script.jlink.tmp";
        const string LOG_FILE = "logs.txt";
        IniData config;
        string jlink;
        Boolean force = false;

        public Form1()
        {
            InitializeComponent();
        }

        private void btnStart_Click(object sender, EventArgs e)
        {
            btnStart.Enabled = false;
            
            if (serial_number.max > 0)
            {
                // Do work
                string jlink_script = File.ReadAllText(JLINK_TEMPLATE_FILE);
                string serial_string = serial_number.start.ToString("D" + serial_number.length);
                byte[] serial_hex = Encoding.Default.GetBytes(serial_string);
                File.WriteAllText(JLINK_SCRIPT_FILE, jlink_script);
                for (Byte i = 0; i < serial_number.length; i++)
                {
                    File.AppendAllText(JLINK_SCRIPT_FILE, "\r\nw1 0x" + (serial_number.address + i).ToString("X") + ", 0x" + serial_hex[i].ToString("X2"));
                }
                if (force)
                {
                    File.AppendAllText(JLINK_SCRIPT_FILE, "\r\nr\r\n");
                }
                File.AppendAllText(JLINK_SCRIPT_FILE, "\r\nexit\r\n");


                // Execute the command synchronously.
                ExecuteCommand exe = new ExecuteCommand();
                string result = exe.ExecuteCommandSync(jlink + " -CommanderScript " + JLINK_SCRIPT_FILE);
                File.AppendAllText(LOG_FILE, result);
                File.Delete(JLINK_SCRIPT_FILE);

                if (result.Contains("Connecting to J-Link via USB...FAILED: Cannot connect to J-Link via USB."))
                {
                    MessageBox.Show("Cannot connect to J-Link via USB!");
                }

                if (result.Contains("Cannot connect to target."))
                {
                    MessageBox.Show("Cannot connect to target!");
                }

                // Update ini file
                serial_number.start++;
                serial_number.max--;
                config["SerialNo"]["Start"] = serial_number.start.ToString();
                config["SerialNo"]["Max"] = serial_number.max.ToString();
                var parser = new FileIniDataParser();
                parser.WriteFile(CONFIGURATION_FILE, config);

                lbSerialNo.Text = serial_number.start.ToString("D" + serial_number.length);

                if (serial_number.max <= 0) {
                    MessageBox.Show("Maximum count reached!");
                    btnStart.Enabled = false;
                }
                else if (lbSerialNo.Text.Length > serial_number.length)
                {
                    MessageBox.Show("Out of range!");
                    btnStart.Enabled = false;
                }
                else
                {
                    btnStart.Enabled = true;
                }
            }
            else
            {
                // Should never reach here
                MessageBox.Show("Maximum count reached!");
            }
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            var parser = new FileIniDataParser();
            if (File.Exists(CONFIGURATION_FILE))
            {
                config = parser.ReadFile(CONFIGURATION_FILE);
                serial_number.start = UInt64.Parse(config["SerialNo"]["Start"]);
                serial_number.max = UInt64.Parse(config["SerialNo"]["Max"]);
                serial_number.length = Byte.Parse(config["SerialNo"]["Length"]);
                serial_number.address = Convert.ToUInt64(config["SerialNo"]["Address"], 16);
                Console.WriteLine("Address" + serial_number.address);
                jlink = config["System"]["JLINK"];
                force = Convert.ToBoolean(config["System"]["Force"]);
                Console.WriteLine("Force:" + force);
                if (jlink == null)
                {
                    jlink = "JLink";
                }
                lbSerialNo.Text = serial_number.start.ToString("D" + serial_number.length);

                if (serial_number.max <= 0)
                {
                    MessageBox.Show("Maximum count reached!");
                    btnStart.Enabled = false;
                }
                else if (lbSerialNo.Text.Length > serial_number.length)
                {
                    MessageBox.Show("Out of range!");
                    btnStart.Enabled = false;
                }
            }
            else
            {
                MessageBox.Show("Could not find configuration file");
                System.Windows.Forms.Application.Exit();
            }
        }
    }
}
