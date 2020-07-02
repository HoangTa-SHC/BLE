using System;
using System.Collections.Generic;
using System.ComponentModel;
//using System.Data;
//using System.Drawing;
using System.IO;
using System.IO.Ports;
using System.Windows.Forms;
using System.Diagnostics;
using System.Timers;

namespace VirtualSerialDevice
{
    public partial class Form1 : Form
    {
        enum SensorType
        {
            AD_TEMP_HUM,
            AD_ACCEL,
            AD_NONE,
        }

        public const int SERIAL_BAUD_RATE = 115200;
        const string UART_LOG_FILE_PATH = "serial.log"; // File to save log from Uart RichTextBox
        const string STATUS_LOG_FILE_PATH = "status.log"; // File to save log from Status RichTextBox
        const string HEX_FILE_PATH = "hex.txt";
        const string HEX_CSV_FILE_PATH = "hex.csv";
        const string BIN_FILE_PATH = "bin.txt";
        const string BIN_CSV_FILE_PATH = "bin.csv";
        const string REPORT_FILE_PATH = "report.log";
        private int data_type_flag = (int)SensorType.AD_NONE;
        private string timeStam;

        // Offset header
        private const int _MUN_CODE_ = 0; // 2-byte length
        private const int _STATUS_ = _MUN_CODE_ + (2 * 2); // 2-byte length
        private const int _SENSOR_DATA_ = _STATUS_ + (2 * 2); // 14-byte or 22-byte length (USER DATA)

        // Sensor value - User data
        private const int _ACCEL_X1_ = 0; // 2-byte length . _SENSOR_DATA_ start
        private const int _ACCEL_Y1_ = _ACCEL_X1_ + (2 * 2); // 2-byte length
        private const int _ACCEL_Z1_ = _ACCEL_Y1_ + (2 * 2); // 2-byte length
        private const int _ACCEL_X2_ = _ACCEL_Z1_ + (2 * 2); // 2-byte length
        private const int _ACCEL_Y2_ = _ACCEL_X2_ + (2 * 2); // 2-byte length
        private const int _ACCEL_Z2_ = _ACCEL_Y2_ + (2 * 2); // 2-byte length
        private const int _ACCEL_X3_ = _ACCEL_Z2_ + (2 * 2); // 2-byte length
        private const int _ACCEL_Y3_ = _ACCEL_X3_ + (2 * 2); // 2-byte length
        private const int _ACCEL_Z3_ = _ACCEL_Y3_ + (2 * 2); // 2-byte length
        private const int _ACCEL_LSEQ_ = _ACCEL_Z3_ + (2 * 2); // 2-byte length
        private const int _FRAME_LEN_ACCEL_ = _ACCEL_LSEQ_ + (2 * 2); // AD-ACCEL frame length

        // Sensor value - User data
        private const int _AD1_ = 0; // 2-byte length , _SENSOR_DATA_ start
        private const int _AD2_ = _AD1_ + (2 * 2); // 2-byte length
        private const int _AD_H = _AD2_ + (2 * 2); // 2-byte length
        private const int _ADTH_LSEQ_ = _AD_H + (2 * 2); // 2-byte length
        private const int _LEN_ = _ADTH_LSEQ_ + (2 * 2); // 1-byte length
        private const int _TYPE_ = _LEN_ + (1 * 2); // 1-byte length
        private const int _DEVICE_NAME_ = _TYPE_ + (1 * 2); // 7-byte length
        private const int _FRAME_LEN_ADTH_ = _DEVICE_NAME_ + (7 * 2); // AD-TEMP and AD-HUM frame length
        

        // Report lost packet variables
        private int RESEND_NUM = 0; // 3 get from TextBox
        private int TOTAL_PACK_NUM = 0;  // 8 get from TextBox
        private int SEQ_NUM_RANGE = 0;  // [0:4] get from TextBox
        private int resend_cnt = 0;
        private int expected_pack_num = 0;
        private int prev_pack_num = 0;
        private int prev_lseq = 0;
        private int prev_seq_num = 0;
        private string prev_str = "";
        private int total_lost_packet_num = 0;
        private int total_recv_packet_num = 0;

        // Timer
        Stopwatch stopWatch = new Stopwatch();
        private static System.Timers.Timer aTimer;
        
        static SerialPort _serialPort;

        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            btnClose.Enabled = false; // Disable Close button

            string[] ports = SerialPort.GetPortNames();

            foreach (string port in ports)
            {
                cbSerialPort.Items.Add(port);
            }

            if (ports.Length > 0)
            {
                cbSerialPort.SelectedIndex = 0;
            }
            else
            {
                btnConnect.Enabled = false;
            }
        }

        private void BtnConnect_Click(object sender, EventArgs e)
        {
            _serialPort = new SerialPort();
            _serialPort.BaudRate = SERIAL_BAUD_RATE;
            _serialPort.PortName = cbSerialPort.Text;
            _serialPort.DataReceived += new SerialDataReceivedEventHandler(DataReceivedHandler);

            try
            {
                Console.WriteLine("Connect button pressed");

                _serialPort.Open();
                btnConnect.Enabled = false;
                cbSerialPort.Enabled = false;
                btnClose.Enabled = true;

                printStatus("Open serial " + _serialPort.PortName + ", " + _serialPort.BaudRate + ", " + _serialPort.Parity + ", " + _serialPort.DataBits + ", " + _serialPort.StopBits);

                // Get Packet varibles
                RESEND_NUM = Convert.ToInt32(txtBx_ReSendNum.Text);
                TOTAL_PACK_NUM = Convert.ToInt32(txtBx_TotalPacketNum.Text);
                SEQ_NUM_RANGE = Convert.ToInt32(txtBxSeqNum.Text);
                SetTextConsole("\n" + lbl_resend.Text + RESEND_NUM);
                SetTextConsole("\n" + lbl_package.Text + TOTAL_PACK_NUM);
                SetTextConsole("\n" + label11.Text + SEQ_NUM_RANGE);
                SetTextConsole("\n" + label1.Text + txtBxPrefix.Text);

                // Disable input TextBox
                txtBx_ReSendNum.Enabled = false;
                txtBx_TotalPacketNum.Enabled = false;
                txtBxSeqNum.Enabled = false;
                txtBxPrefix.Enabled = false;

                // Delete all files in a directory    
                string[] files = { UART_LOG_FILE_PATH, STATUS_LOG_FILE_PATH , REPORT_FILE_PATH, HEX_FILE_PATH, HEX_CSV_FILE_PATH , BIN_FILE_PATH, BIN_CSV_FILE_PATH };
                foreach (string file in files)
                {
                    File.Delete(file);
                    //printStatus($"{file} is deleted.");
                }

                // Reset variables
                resend_cnt = 0;
                txtBx_cnt.Text = resend_cnt.ToString();
                prev_pack_num = 0;
                txtBoxPacket.ResetText();
                expected_pack_num = 0;
                txtBxExpected.ResetText();
                lblWarning.ResetText();
                total_lost_packet_num = 0;
                txtBxTotalLost.Text = total_lost_packet_num.ToString();
                lblTotalLostPercent.ResetText();
                total_recv_packet_num = 0;
                txtBxTotalRecv.Text = total_recv_packet_num.ToString();
                lblTotalReceivedPercent.ResetText();
                prev_lseq = 0;
                prev_seq_num = 0;
                prev_str = "";

                // Set Timer
                SetTimer();
                stopWatch.Start();
                lblRunTime.ResetText();
            }
            catch (Exception) { };

        }

        private void btnClose_Click(object sender, EventArgs e)
        {
            try
            {
                _serialPort.Close();
                btnConnect.Enabled = true;
                btnClose.Enabled = false;
                cbSerialPort.Enabled = true;

                // Enable input TextBoxs
                txtBxPrefix.Enabled = true;
                txtBx_ReSendNum.Enabled = true;
                txtBx_TotalPacketNum.Enabled = true;
                txtBxSeqNum.Enabled = true;

                printStatus("Close serial " + _serialPort.PortName);

                // Stop Timer
                StopTimer();
                stopWatch.Stop();
                stopWatch.Reset();

                // Display to Report RichTextBox
                if(total_recv_packet_num > 0)
                {
                    SetTextReport("Total Received Packet: " + total_recv_packet_num + ", Total Lost Packet: " + total_lost_packet_num);
                }
            }
            catch (Exception) { };
        }

        private void btnClear_Click(object sender, EventArgs e)
        {
            richTextBox_console.Clear();
            richTextBox_uart.Clear();
            rTxtBxReport.Clear();
        }

        private void btnSave_Click(object sender, EventArgs e)
        {
            SetTextConsole("\nSave all log\n");

            // Save Status RichTextBox
            using (StreamWriter sw = File.CreateText(STATUS_LOG_FILE_PATH))
            {
                sw.Write(richTextBox_console.Text);
            }

            // Save Serial RichTextBox
            using (StreamWriter sw = File.CreateText(UART_LOG_FILE_PATH))
            {
                sw.Write(richTextBox_uart.Text);
            }

            // Save Report RichTextBox
            using (StreamWriter sw = File.CreateText(REPORT_FILE_PATH))
            {
                sw.Write(rTxtBxReport.Text);
            }
        }

        // Callback Handler for richTextBox_uart
        // https://stackoverflow.com/questions/10775367/cross-thread-operation-not-valid-control-textbox1-accessed-from-a-thread-othe
        delegate void SetTextUartCallback(string text);
        private void SetTextUart(string text)
        {
            // InvokeRequired required compares the thread ID of the
            // calling thread to the thread ID of the creating thread.
            // If these threads are different, it returns true.
            if (this.richTextBox_uart.InvokeRequired)
            {
                SetTextUartCallback d = new SetTextUartCallback(SetTextUart);
                this.Invoke(d, new object[] { text });
            }
            else
            {
                this.richTextBox_uart.AppendText(text);

                // Save serial log
                using (StreamWriter sw = File.AppendText(UART_LOG_FILE_PATH))
                {
                    sw.Write(text);
                }
            }
        }

        // Callback Handler for richTextBox_console
        delegate void SetTextConsoleCallback(string text);
        private void SetTextConsole(string text)
        {
            // InvokeRequired required compares the thread ID of the
            // calling thread to the thread ID of the creating thread.
            // If these threads are different, it returns true.
            if (this.richTextBox_console.InvokeRequired)
            {
                SetTextConsoleCallback d = new SetTextConsoleCallback(SetTextConsole);
                this.Invoke(d, new object[] { text });
            }
            else
            {
                this.richTextBox_console.AppendText(text);
                Console.Write(text); // Debug

                // Save log
                using (StreamWriter sw = File.AppendText(STATUS_LOG_FILE_PATH))
                {
                    sw.Write(text);
                }
            }
        }

        // Callback Handler for rTxtBxReport
        delegate void SetTextReportCallback(string text);
        private void SetTextReport(string text)
        {
            // InvokeRequired required compares the thread ID of the
            // calling thread to the thread ID of the creating thread.
            // If these threads are different, it returns true.
            if (this.rTxtBxReport.InvokeRequired)
            {
                SetTextReportCallback d = new SetTextReportCallback(SetTextReport);
                this.Invoke(d, new object[] { text });
            }
            else
            {
                this.rTxtBxReport.AppendText(text);

                // Save report log
                using (StreamWriter sw = File.AppendText(REPORT_FILE_PATH))
                {
                    sw.Write(text);
                }
            }
        }

        // Callback Handler for txtBx_cnt
        delegate void SetTextBoxCntCallback(string text);
        private void SetTextBoxCnt(string text)
        {
            // InvokeRequired required compares the thread ID of the
            // calling thread to the thread ID of the creating thread.
            // If these threads are different, it returns true.
            if (this.txtBx_cnt.InvokeRequired)
            {
                SetTextBoxCntCallback d = new SetTextBoxCntCallback(SetTextBoxCnt);
                this.Invoke(d, new object[] { text });
            }
            else
            {
                this.txtBx_cnt.Text = text;
            }
        }

        // Callback Handler for txtBx_cnt
        delegate void SetTextBoxPacketCallback(string text);
        private void SetTextBoxPacket(string text)
        {
            // InvokeRequired required compares the thread ID of the
            // calling thread to the thread ID of the creating thread.
            // If these threads are different, it returns true.
            if (this.txtBoxPacket.InvokeRequired)
            {
                SetTextBoxPacketCallback d = new SetTextBoxPacketCallback(SetTextBoxPacket);
                this.Invoke(d, new object[] { text });
            }
            else
            {
                this.txtBoxPacket.Text = text;
            }
        }
       
        // Callback Handler for txtBxExpected
        delegate void SetTextBoxExpectedCallback(string text);
        private void SetTextBoxExpected(string text)
        {
            // InvokeRequired required compares the thread ID of the
            // calling thread to the thread ID of the creating thread.
            // If these threads are different, it returns true.
            if (this.txtBxExpected.InvokeRequired)
            {
                SetTextBoxExpectedCallback d = new SetTextBoxExpectedCallback(SetTextBoxExpected);
                this.Invoke(d, new object[] { text });
            }
            else
            {
                this.txtBxExpected.Text = text;
            }
        }

        // Callback Handler for lblWarning
        delegate void SetTextLabelWarningCallback(string text);
        private void SetTextLabelWarning(string text)
        {
            // InvokeRequired required compares the thread ID of the
            // calling thread to the thread ID of the creating thread.
            // If these threads are different, it returns true.
            if (this.lblWarning.InvokeRequired)
            {
                SetTextLabelWarningCallback d = new SetTextLabelWarningCallback(SetTextLabelWarning);
                this.Invoke(d, new object[] { text });
            }
            else
            {
                this.lblWarning.Text = text;
            }
        }

        // Callback Handler for txtBxTotalLost
        delegate void SetTextBoxTotalLostCallback(string text);
        private void SetTextBoxTotalLost(string text)
        {
            // InvokeRequired required compares the thread ID of the
            // calling thread to the thread ID of the creating thread.
            // If these threads are different, it returns true.
            if (this.txtBxTotalLost.InvokeRequired)
            {
                SetTextBoxTotalLostCallback d = new SetTextBoxTotalLostCallback(SetTextBoxTotalLost);
                this.Invoke(d, new object[] { text });
            }
            else
            {
                this.txtBxTotalLost.Text = text;
            }
        }

        // Callback Handler for txtBxTotalRecv
        delegate void SetTextBoxTotalRecviveCallback(string text);
        private void SetTextBoxTotalRecvive(string text)
        {
            // InvokeRequired required compares the thread ID of the
            // calling thread to the thread ID of the creating thread.
            // If these threads are different, it returns true.
            if (this.txtBxTotalRecv.InvokeRequired)
            {
                SetTextBoxTotalRecviveCallback d = new SetTextBoxTotalRecviveCallback(SetTextBoxTotalRecvive);
                this.Invoke(d, new object[] { text });
            }
            else
            {
                this.txtBxTotalRecv.Text = text;
            }
        }

        // Callback Handler for lblRunTime
        delegate void SetTextLabelRunTimeCallback(string text);
        private void SetTextLabelRunTime(string text)
        {
            // InvokeRequired required compares the thread ID of the
            // calling thread to the thread ID of the creating thread.
            // If these threads are different, it returns true.
            if (this.lblRunTime.InvokeRequired)
            {
                SetTextLabelRunTimeCallback d = new SetTextLabelRunTimeCallback(SetTextLabelRunTime);
                this.Invoke(d, new object[] { text });
            }
            else
            {
                this.lblRunTime.Text = text;
            }
        }

        // Callback Handler for lblTotalLostPercent
        delegate void SetTextLabelLostPercentCallback(string text);
        private void SetTextLabelLostPercent(string text)
        {
            // InvokeRequired required compares the thread ID of the
            // calling thread to the thread ID of the creating thread.
            // If these threads are different, it returns true.
            if (this.lblTotalLostPercent.InvokeRequired)
            {
                SetTextLabelLostPercentCallback d = new SetTextLabelLostPercentCallback(SetTextLabelLostPercent);
                this.Invoke(d, new object[] { text });
            }
            else
            {
                this.lblTotalLostPercent.Text = text;
            }
        }

        // Callback Handler for lblTotalReceivedPercent
        delegate void SetTextLabelRecvPercentCallback(string text);
        private void SetTextLabelRecvPercent(string text)
        {
            // InvokeRequired required compares the thread ID of the
            // calling thread to the thread ID of the creating thread.
            // If these threads are different, it returns true.
            if (this.lblTotalReceivedPercent.InvokeRequired)
            {
                SetTextLabelRecvPercentCallback d = new SetTextLabelRecvPercentCallback(SetTextLabelRecvPercent);
                this.Invoke(d, new object[] { text });
            }
            else
            {
                this.lblTotalReceivedPercent.Text = text;
            }
        }

        private void DataReceivedHandler(object sender, SerialDataReceivedEventArgs e)
        {
            SerialPort sp = (SerialPort)sender;
            //string indata = sp.ReadExisting();

            while (sp.BytesToRead > 0)
            {
                string indata = sp.ReadLine();

            // Print uart received
                SetTextUart(indata);

                // Get timeStamp
                timeStam = DateTime.Now.ToString("yyyy/MM/dd HH:mm:ss.ff ");
                // Print status
                SetTextConsole("\n" + timeStam + "Received Lenghth: " + indata.Length + "\n");


                string prefix_str = txtBxPrefix.Text; //"ManufacturerData=";
                string hex_str = "";
                string bin_str = "";
                string analyze_str = "";

                if (indata.Contains(prefix_str))
                {
                    hex_str = indata.Substring(prefix_str.Length);  // remove prefix string
                    hex_str = hex_str.Replace("\n", String.Empty);  // remove \r
                    hex_str = hex_str.Replace("\r", String.Empty);  // remove \n
                    bin_str = Hex2Bin_String(hex_str);
                    
                    // Analyze BLE data
                    analyze_str = analyze(hex_str);
                    SetTextConsole("Hex: " + hex_str + "\n" + "Bin: " + bin_str + "\n" + analyze_str + "\n");
                } else
                {
                    SetTextConsole("Not found prefix: " + prefix_str);
                }

                // Save hex string, bin string
                if (indata.Contains(prefix_str))
                {
                    using (StreamWriter sw = File.AppendText(HEX_FILE_PATH))
                    {
                        sw.WriteLine(hex_str);
                    }
                    using (StreamWriter sw = File.AppendText(BIN_FILE_PATH))
                    {
                        sw.WriteLine(bin_str);
                    }
                    using (StreamWriter sw = File.AppendText(HEX_CSV_FILE_PATH))
                    {
                        for (int i = 0; i < hex_str.Length; i += 2)
                        {
                            // Add ", " comma and space separating
                            // XXXXXXXX -> XX, XX, XX, XX, 
                            string s1 = hex_str.Substring(i, 2);
                            sw.Write(s1);
                            sw.Write(", "); // add separator
                        }
                        sw.Write("\n");
                    }
                    using (StreamWriter sw = File.AppendText(BIN_CSV_FILE_PATH))
                    {
                        for (int i = 0; i < bin_str.Length; i += 2 * 4)
                        {
                            // Add ", " comma and space separating
                            // XXXXXXXX -> XXXX, XXXX, 
                            string s1 = bin_str.Substring(i, 2 * 4);
                            sw.Write(s1);
                            sw.Write(", "); // add separator
                        }
                        sw.Write("\n");
                    }
                }
            }
        }

        private string analyze(string hex_str)
        {
            string ret = "";

            // MUN-CODE : first 4-hex-digit
            string munCode = hex_str.Substring(_MUN_CODE_, 4);

            // STATUS : next 4-hex-digit
            string D15D0 = swapByte(hex_str.Substring(_STATUS_, 4)); // status: [D7D0 D15D8] -> [D15D8 D7D0]
            string D15D0_bin_str = Hex2Bin_String(D15D0);

            // DATA-TYPE
            data_type_flag = (int)igetDataType(D15D0_bin_str.Substring(12,4)); // [D3:D0]
            string data_type = getDataTypeString((SensorType)data_type_flag);

            // SEQ-NUM
            int seq_num = getSeqNum(D15D0_bin_str.Substring(8, 4)); // [D7:D4]

            // BLOCK-NUM
            int block_num = getBlockNum(D15D0_bin_str.Substring(2, 3)); // [D13:D11]

            string status_str = "Status: [D15:D0] " + D15D0 + " (" + Hex2Bin_String(D15D0) + ")" // [D15: D8] 00110000 , [D7:D0] 00111100
                              + ", SeqNum: " + seq_num + ", DataType: " + data_type + ", BlockNum: " + block_num;

            // USER-DATA or SENSOR-VALUE
            string user_data = hex_str.Substring(_SENSOR_DATA_); // sensor data

            // Analyze sensor-data : AD-ACCEL, AD-TEMP, AD-HUMID
            string[] sensor_data = getSensorData(user_data, (SensorType)data_type_flag, block_num);
            int[] i_sensor_data = new int[10];
            string lseq_str = getLSeq(sensor_data, (SensorType)data_type_flag, block_num); // L-SEQ hex string
            int lseq = Convert.ToInt32(lseq_str, 16); // Convert hex string to integer value

            string sensor_str = "";
            float humid = (float)Convert.ToInt32(sensor_data[2], 16) / 65536; // calculate humid value (%)
            
            if (data_type_flag == (int)SensorType.AD_TEMP_HUM)
            {
                // Set sensor XYZ string
                sensor_str = "LSeq: " + lseq + "(" + lseq_str + "), "
                    + "AD1: " + Convert.ToInt32(sensor_data[0], 16) + "(" + sensor_data[0] + "), "  // AD1
                    + "AD2: " + Convert.ToInt32(sensor_data[1], 16) + "(" + sensor_data[1] + "), "  // AD2
                    + "AD_H: " + String.Format("{0:P2}", humid) + "(" + sensor_data[2] + "), "  // AD-H
                    + "LEN: " + Convert.ToInt32(sensor_data[4], 16) + "(" + sensor_data[4] + ") ";  // LEN
            }
            else if (data_type_flag == (int)SensorType.AD_ACCEL)
            {
                // Set sensor XYZ string
                sensor_str = "LSeq: " + lseq + " (" + lseq_str + ") "
                    + String.Format("\n X{0} Y{0} Z{0}", block_num * 3 + 1)
                    + String.Format(" | {0:X4} {1:X4} {2:X4}", sensor_data[0], sensor_data[1], sensor_data[2])
                    + String.Format(" | {0} {1} {2}", Convert.ToInt32(sensor_data[0], 16), Convert.ToInt32(sensor_data[1], 16), Convert.ToInt32(sensor_data[2], 16))
                    + String.Format("\n X{0} Y{0} Z{0}", block_num * 3 + 2)
                    + String.Format(" | {0:X4} {1:X4} {2:X4}", sensor_data[3], sensor_data[4], sensor_data[5])
                    + String.Format(" | {0} {1} {2}", Convert.ToInt32(sensor_data[3], 16), Convert.ToInt32(sensor_data[4], 16), Convert.ToInt32(sensor_data[5], 16));

                if (block_num < 6)
                {
                    sensor_str += String.Format("\n X{0} Y{0} Z{0}", block_num * 3 + 3)
                        + String.Format(" | {0:X4} {1:X4} {2:X4}", sensor_data[6], sensor_data[7], sensor_data[8])
                        + String.Format(" | {0} {1} {2}", Convert.ToInt32(sensor_data[6], 16), Convert.ToInt32(sensor_data[7], 16), Convert.ToInt32(sensor_data[8], 16));
                }
            }

            // Compare current and previous string
            string curr_str = hex_str;
            int isDifferent = String.Compare(curr_str, prev_str); // 0: equal, ortherwise: differrent
            if (isDifferent == 0)
            {
                // Strings are the same
                resend_cnt++;            // Update resend counter
            }
            else
            {
                // Strings are different
                resend_cnt = 1;          // Reset resend counter
                total_recv_packet_num++; // Update total received packet
            }

            // Get packet_num : current Packet number depend on block_num and data_type
            int packet_num = getPacketNum((SensorType)data_type_flag, block_num);
            string cnt_str = "(" + packet_num + "/" + TOTAL_PACK_NUM + "), (" + resend_cnt + "/" + RESEND_NUM + ")";

            // Update counter expected_pack_num
            if (isDifferent != 0 && expected_pack_num != packet_num)//|| resend_cnt >= RESEND_NUM)
            {
                expected_pack_num++; // increase counter
                if (expected_pack_num > TOTAL_PACK_NUM)
                {
                    expected_pack_num = 1; // reset
                }
            }

            // Calculate lost packet
            // Calculate base on lseq, seq_num, packet_num, TOTAL_PACK_NUM = 8 (1:8), SEQ_NUM_RANGE = 4 (0:3)
            // curr = 8 * 4 * lseq + 8 * seq_num + (packet_num - 1);
            int curr = TOTAL_PACK_NUM * SEQ_NUM_RANGE * lseq + TOTAL_PACK_NUM * seq_num + (packet_num - 1);
            int prev = TOTAL_PACK_NUM * SEQ_NUM_RANGE * prev_lseq + TOTAL_PACK_NUM * prev_seq_num + (prev_pack_num - 1);
            int sub = curr - prev;
            if (sub == 1)
            {
                SetTextLabelWarning("success");
            }
            else if (sub >= 2 && prev > 0)
            {
                // have lost packet
                total_lost_packet_num += sub - 1;
                SetTextLabelWarning("failed");
            }
            else
            {
                // error < 0
            }

            // Calculate lost/received percentage
            float lost_percent = (float)total_lost_packet_num / (float)(total_lost_packet_num + total_recv_packet_num);
            float recv_percent = (float)total_recv_packet_num / (float)(total_lost_packet_num + total_recv_packet_num);
            string lost_percent_str = String.Format("{0:P2}", lost_percent); // example: 12.34%
            string recv_percent_str = String.Format("{0:P2}", recv_percent);

            // Display to Text Box
            SetTextBoxCnt(resend_cnt.ToString());
            SetTextBoxExpected(expected_pack_num.ToString());
            SetTextBoxPacket(packet_num.ToString());
            SetTextBoxTotalRecvive(total_recv_packet_num.ToString());
            SetTextBoxTotalLost(total_lost_packet_num.ToString());

            // Display to Label
            SetTextLabelLostPercent(lost_percent_str);
            SetTextLabelRecvPercent(recv_percent_str);

            // Create Report log
            string report_str = timeStam + "Packet LSeq: " + lseq + ", seq_num: " + seq_num + ", " + cnt_str
                             + ", received: " + packet_num + ", expected: " + expected_pack_num + ", total received: " + total_recv_packet_num + " (" + recv_percent_str + ")";

            if (sub >= 2 && prev > 0)
            {
                // have lost packet
                report_str += ", total lost: " + total_lost_packet_num + " (" + lost_percent_str + ")";
            }

            // Display to Report RichTextBox
            SetTextReport(report_str + "\n");

            // Save to previous variables
            prev_lseq = lseq;
            prev_seq_num = seq_num;
            prev_pack_num = packet_num;
            expected_pack_num = packet_num; // Update expected_pack_num for next receiving
            prev_str = curr_str;

            // Set return string
            ret = "MunCode: " + munCode
                // 3C30 (0011000000111100)
                + "\n" + status_str
                + "\nUserData: " + user_data
                //  (1/3) (2/22)
                + "\n  " + cnt_str
                // X1   Y1   Z1 | HHHH HHHH HHHH | ddd  ddd  ddd  
                + ", " + sensor_str;
            return ret;
        }

        private string Hex2Bin_String(string hex_str)
        {
            string bin_str = "";
            foreach (char charHex in hex_str.ToUpper().ToCharArray())
            {
                // Convert a hexadecimal character to an binary string.
                bin_str = bin_str + Hex2Bin(charHex);
            }
            return bin_str;
        }

        private string Hex2Bin(char hex)
        {
            switch (hex)
            {
                case '0': { return "0000"; break; }
                case '1': { return "0001"; break; }
                case '2': { return "0010"; break; }
                case '3': { return "0011"; break; }
                case '4': { return "0100"; break; }
                case '5': { return "0101"; break; }
                case '6': { return "0110"; break; }
                case '7': { return "0111"; break; }
                case '8': { return "1000"; break; }
                case '9': { return "1001"; break; }
                case 'A': case 'a': { return "1010"; break; }
                case 'B': case 'b': { return "1011"; break; }
                case 'C': case 'c': { return "1100"; break; }
                case 'D': case 'd': { return "1101"; break; }
                case 'E': case 'e': { return "1110"; break; }
                case 'F': case 'f': { return "1111"; break; }
                default: { return hex.ToString(); break; } // return itself character
            }
        }

        private SensorType igetDataType(string D3D0_bin_str)
        {
            char [] arr = D3D0_bin_str.ToCharArray();
            //if (arr[0] == '1')  // D3 : DEVICE-NAME  (Should be always "1")
            if (arr[1] == '1') { return SensorType.AD_ACCEL; } // D2
            if (arr[2] == '1') { return SensorType.AD_TEMP_HUM; } // D1
            if (arr[3] == '1') { return SensorType.AD_TEMP_HUM; } // D0
            return SensorType.AD_NONE;
        }

        private string getDataTypeString(SensorType sensor_type)
        {
            if (sensor_type == SensorType.AD_TEMP_HUM)
            {
                return "AD-TEMP AD-HUM ";
            }
            else if (sensor_type == SensorType.AD_ACCEL)
            {
                return "AD-ACCEL";
            }
            return null;
        }

        // SEQ-NUM (0-15) [D7:D4]
        private int getSeqNum(string D7D4_bin_str)
        {
            int val = Convert.ToInt32(D7D4_bin_str, 2); // Convert binary string to integer value
            return val;
        }

        // Block number of AD-ACCEL (0-6) [D7:D4]
        private int getBlockNum(string D13D11_bin_str)
        {
            int val = Convert.ToInt32(D13D11_bin_str, 2); // Convert binary string to integer value
            return val;
        }

        private string[] getSensorData(string hex_str, SensorType sensor_type, int block_num)
        {
            string[] str_arr = new string[10];
            if (sensor_type == SensorType.AD_TEMP_HUM)
            {
                // AD1 AD2 AD-H
                str_arr[0] = swapByte(hex_str.Substring(_AD1_, 4));
                str_arr[1] = swapByte(hex_str.Substring(_AD2_, 4));
                str_arr[2] = swapByte(hex_str.Substring(_AD_H, 4));

                // L-SEQ
                str_arr[3] = swapByte(hex_str.Substring(_ADTH_LSEQ_, 4));

                // LEN
                str_arr[4] = hex_str.Substring(_LEN_, 2);

                //// TYPE
                //str_arr[5] = hex_str.Substring(_TYPE_, 2);

                //// DEVICE-NAME
                //str_arr[6] = hex_str.Substring(_DEVICE_NAME_, 7*2);
            }
            else
            if (sensor_type == SensorType.AD_ACCEL)
            {
                // 1st XYZ
                str_arr[0] = swapByte(hex_str.Substring(_ACCEL_X1_, 4));
                str_arr[1] = swapByte(hex_str.Substring(_ACCEL_Y1_, 4));
                str_arr[2] = swapByte(hex_str.Substring(_ACCEL_Z1_, 4));

                // 2nd XYZ
                str_arr[3] = swapByte(hex_str.Substring(_ACCEL_X2_, 4));
                str_arr[4] = swapByte(hex_str.Substring(_ACCEL_Y2_, 4));
                str_arr[5] = swapByte(hex_str.Substring(_ACCEL_Z2_, 4));

                if (block_num < 6)
                {
                    // 3rd XYZ
                    str_arr[6] = swapByte(hex_str.Substring(_ACCEL_X3_, 4));
                    str_arr[7] = swapByte(hex_str.Substring(_ACCEL_Y3_, 4));
                    str_arr[8] = swapByte(hex_str.Substring(_ACCEL_Z3_, 4));

                    // L-SEQ
                    str_arr[9] = swapByte(hex_str.Substring(_ACCEL_LSEQ_, 4));
                } else
                {
                    // Ignore 3rd XYZ if block_num = 6
                    // L-SEQ
                    str_arr[6] = swapByte(hex_str.Substring(_ACCEL_X3_, 4));
                }
            }
            return str_arr;
        }

        // swap 2 byte for 4-digit hexdecimal string
        // retrun null if false
        private string swapByte(string hex_4_digits_str)
        {
            if (hex_4_digits_str.Length == 4)
            {
                string low = hex_4_digits_str.Substring(0, 2); // low digit
                string high = hex_4_digits_str.Substring(2, 2); // hight digit
                return (high + low);
            }
            else return null;
        }

        private string getLSeq(string[] sensor_data_arr, SensorType sensor_type, int block_num)
        {
            string s = "0";
            if(sensor_data_arr.Length == 10)
            {
                if (data_type_flag == (int)SensorType.AD_TEMP_HUM)
                {
                    s = sensor_data_arr[3]; // L-SEQ hex string
                }
                else if (data_type_flag == (int)SensorType.AD_ACCEL)
                {
                    if (block_num < 6)
                    {
                        s = sensor_data_arr[9]; // L-SEQ hex string
                    }
                    else
                    {
                        s = sensor_data_arr[6];
                    }
                }
            }
            return s;
        }

        private int getPacketNum(SensorType senror_type, int block_num)
        {
            if (senror_type == SensorType.AD_TEMP_HUM)
            {
                return 1; // ad temp-hum : P(1)
            }
            else if (senror_type == SensorType.AD_ACCEL)
            {
                return block_num + 2; // block_num(0-6) : P(2-8)
            }
            return -1;
        }

        private void printStatus(string str)
        {
            // Print timeStamp
            // https://stackoverflow.com/questions/21953090/c-sharp-time-stamp-issue
            SetTextConsole(DateTime.Now.ToString("yyyy/MM/dd HH:mm:ss.ff "));
            // Print status
            SetTextConsole(str + "\n");
        }

        //
        // System Timer
        //
        private void SetTimer()
        {
            // Create a timer with a 100 milisecond interval.
            aTimer = new System.Timers.Timer(100);
            // Hook up the Elapsed event for the timer. 
            aTimer.Elapsed += OnTimedEvent;
            aTimer.AutoReset = true;
            aTimer.Enabled = true;
        }

        private void StopTimer()
        {
            aTimer.Stop();
            aTimer.Dispose();
        }

        private void OnTimedEvent(Object source, ElapsedEventArgs e)
        {
            //Console.WriteLine("The Elapsed event was raised at {0:HH:mm:ss.fff}", e.SignalTime);
            //SetTextLabelRunTime(string.Format("{0:HH:mm:ss}", e.SignalTime)); // {0:HH:mm:ss.fff} // display current time

            // Get the elapsed time as a TimeSpan value.
            TimeSpan ts = stopWatch.Elapsed;

            // Format and display the TimeSpan value.
            string elapsedTime = String.Format("{0:00}:{1:00}:{2:00}.{3:00}",
                ts.Hours, ts.Minutes, ts.Seconds,
                ts.Milliseconds / 10);
            //Console.WriteLine("RunTime " + elapsedTime);
            SetTextLabelRunTime(elapsedTime);
        }

    }
}
