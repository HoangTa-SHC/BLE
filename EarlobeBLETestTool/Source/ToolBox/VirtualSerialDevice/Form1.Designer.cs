namespace VirtualSerialDevice
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.serialPort1 = new System.IO.Ports.SerialPort(this.components);
            this.cbSerialPort = new System.Windows.Forms.ComboBox();
            this.btnConnect = new System.Windows.Forms.Button();
            this.richTextBox_uart = new System.Windows.Forms.RichTextBox();
            this.txtBxPrefix = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.richTextBox_console = new System.Windows.Forms.RichTextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.btnClose = new System.Windows.Forms.Button();
            this.lbl_resend = new System.Windows.Forms.Label();
            this.lbl_package = new System.Windows.Forms.Label();
            this.txtBx_ReSendNum = new System.Windows.Forms.TextBox();
            this.txtBx_TotalPacketNum = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.txtBx_cnt = new System.Windows.Forms.TextBox();
            this.txtBoxPacket = new System.Windows.Forms.TextBox();
            this.btnSave = new System.Windows.Forms.Button();
            this.btnClear = new System.Windows.Forms.Button();
            this.label4 = new System.Windows.Forms.Label();
            this.rTxtBxReport = new System.Windows.Forms.RichTextBox();
            this.label7 = new System.Windows.Forms.Label();
            this.txtBxExpected = new System.Windows.Forms.TextBox();
            this.lblWarning = new System.Windows.Forms.Label();
            this.txtBxTotalLost = new System.Windows.Forms.TextBox();
            this.label8 = new System.Windows.Forms.Label();
            this.lblRunTime = new System.Windows.Forms.Label();
            this.txtBxTotalRecv = new System.Windows.Forms.TextBox();
            this.label9 = new System.Windows.Forms.Label();
            this.label10 = new System.Windows.Forms.Label();
            this.txtBxSeqNum = new System.Windows.Forms.TextBox();
            this.label11 = new System.Windows.Forms.Label();
            this.lblTotalLostPercent = new System.Windows.Forms.Label();
            this.lblTotalReceivedPercent = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // cbSerialPort
            // 
            this.cbSerialPort.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbSerialPort.FormattingEnabled = true;
            this.cbSerialPort.Location = new System.Drawing.Point(485, 83);
            this.cbSerialPort.Name = "cbSerialPort";
            this.cbSerialPort.Size = new System.Drawing.Size(99, 21);
            this.cbSerialPort.TabIndex = 0;
            // 
            // btnConnect
            // 
            this.btnConnect.Location = new System.Drawing.Point(590, 82);
            this.btnConnect.Name = "btnConnect";
            this.btnConnect.Size = new System.Drawing.Size(75, 23);
            this.btnConnect.TabIndex = 1;
            this.btnConnect.Text = "Connect";
            this.btnConnect.UseVisualStyleBackColor = true;
            this.btnConnect.Click += new System.EventHandler(this.BtnConnect_Click);
            // 
            // richTextBox_uart
            // 
            this.richTextBox_uart.BackColor = System.Drawing.SystemColors.InactiveBorder;
            this.richTextBox_uart.Location = new System.Drawing.Point(485, 150);
            this.richTextBox_uart.Name = "richTextBox_uart";
            this.richTextBox_uart.Size = new System.Drawing.Size(417, 138);
            this.richTextBox_uart.TabIndex = 3;
            this.richTextBox_uart.Text = "";
            this.richTextBox_uart.WordWrap = false;
            // 
            // txtBxPrefix
            // 
            this.txtBxPrefix.Location = new System.Drawing.Point(64, 98);
            this.txtBxPrefix.Name = "txtBxPrefix";
            this.txtBxPrefix.Size = new System.Drawing.Size(137, 20);
            this.txtBxPrefix.TabIndex = 5;
            this.txtBxPrefix.Text = "ManufacturerData=";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(19, 101);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(39, 13);
            this.label1.TabIndex = 6;
            this.label1.Text = "Prefix: ";
            // 
            // richTextBox_console
            // 
            this.richTextBox_console.Location = new System.Drawing.Point(12, 320);
            this.richTextBox_console.Name = "richTextBox_console";
            this.richTextBox_console.Size = new System.Drawing.Size(890, 166);
            this.richTextBox_console.TabIndex = 7;
            this.richTextBox_console.Text = "";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(9, 304);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(40, 13);
            this.label2.TabIndex = 8;
            this.label2.Text = "Status:";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(482, 134);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(51, 13);
            this.label3.TabIndex = 9;
            this.label3.Text = "Uart Log:";
            // 
            // btnClose
            // 
            this.btnClose.Location = new System.Drawing.Point(669, 82);
            this.btnClose.Name = "btnClose";
            this.btnClose.Size = new System.Drawing.Size(75, 23);
            this.btnClose.TabIndex = 10;
            this.btnClose.Text = "Close";
            this.btnClose.UseVisualStyleBackColor = true;
            this.btnClose.Click += new System.EventHandler(this.btnClose_Click);
            // 
            // lbl_resend
            // 
            this.lbl_resend.AutoSize = true;
            this.lbl_resend.Location = new System.Drawing.Point(16, 27);
            this.lbl_resend.Name = "lbl_resend";
            this.lbl_resend.Size = new System.Drawing.Size(93, 13);
            this.lbl_resend.TabIndex = 12;
            this.lbl_resend.Text = "Re-send Number: ";
            // 
            // lbl_package
            // 
            this.lbl_package.AutoSize = true;
            this.lbl_package.Location = new System.Drawing.Point(16, 49);
            this.lbl_package.Name = "lbl_package";
            this.lbl_package.Size = new System.Drawing.Size(114, 13);
            this.lbl_package.TabIndex = 13;
            this.lbl_package.Text = "Total Packet Number: ";
            // 
            // txtBx_ReSendNum
            // 
            this.txtBx_ReSendNum.Location = new System.Drawing.Point(133, 21);
            this.txtBx_ReSendNum.Name = "txtBx_ReSendNum";
            this.txtBx_ReSendNum.Size = new System.Drawing.Size(32, 20);
            this.txtBx_ReSendNum.TabIndex = 14;
            this.txtBx_ReSendNum.Text = "3";
            // 
            // txtBx_TotalPacketNum
            // 
            this.txtBx_TotalPacketNum.Location = new System.Drawing.Point(133, 46);
            this.txtBx_TotalPacketNum.Name = "txtBx_TotalPacketNum";
            this.txtBx_TotalPacketNum.Size = new System.Drawing.Size(32, 20);
            this.txtBx_TotalPacketNum.TabIndex = 15;
            this.txtBx_TotalPacketNum.Text = "8";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(231, 24);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(90, 13);
            this.label5.TabIndex = 16;
            this.label5.Text = "Re-send Counter:";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(265, 49);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(56, 13);
            this.label6.TabIndex = 17;
            this.label6.Text = "Received:";
            // 
            // txtBx_cnt
            // 
            this.txtBx_cnt.Location = new System.Drawing.Point(327, 20);
            this.txtBx_cnt.Name = "txtBx_cnt";
            this.txtBx_cnt.Size = new System.Drawing.Size(62, 20);
            this.txtBx_cnt.TabIndex = 18;
            this.txtBx_cnt.Text = "0";
            // 
            // txtBoxPacket
            // 
            this.txtBoxPacket.Location = new System.Drawing.Point(327, 46);
            this.txtBoxPacket.Name = "txtBoxPacket";
            this.txtBoxPacket.Size = new System.Drawing.Size(62, 20);
            this.txtBoxPacket.TabIndex = 19;
            // 
            // btnSave
            // 
            this.btnSave.Location = new System.Drawing.Point(827, 82);
            this.btnSave.Name = "btnSave";
            this.btnSave.Size = new System.Drawing.Size(75, 23);
            this.btnSave.TabIndex = 20;
            this.btnSave.Text = "Save Log";
            this.btnSave.UseVisualStyleBackColor = true;
            this.btnSave.Click += new System.EventHandler(this.btnSave_Click);
            // 
            // btnClear
            // 
            this.btnClear.Location = new System.Drawing.Point(749, 82);
            this.btnClear.Name = "btnClear";
            this.btnClear.Size = new System.Drawing.Size(75, 23);
            this.btnClear.TabIndex = 21;
            this.btnClear.Text = "Clear Log";
            this.btnClear.UseVisualStyleBackColor = true;
            this.btnClear.Click += new System.EventHandler(this.btnClear_Click);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(9, 134);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(79, 13);
            this.label4.TabIndex = 22;
            this.label4.Text = "Packet Report:";
            // 
            // rTxtBxReport
            // 
            this.rTxtBxReport.Location = new System.Drawing.Point(12, 150);
            this.rTxtBxReport.Name = "rTxtBxReport";
            this.rTxtBxReport.Size = new System.Drawing.Size(464, 139);
            this.rTxtBxReport.TabIndex = 23;
            this.rTxtBxReport.Text = "";
            this.rTxtBxReport.WordWrap = false;
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(266, 75);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(55, 13);
            this.label7.TabIndex = 24;
            this.label7.Text = "Expected:";
            // 
            // txtBxExpected
            // 
            this.txtBxExpected.Location = new System.Drawing.Point(327, 72);
            this.txtBxExpected.Name = "txtBxExpected";
            this.txtBxExpected.Size = new System.Drawing.Size(62, 20);
            this.txtBxExpected.TabIndex = 25;
            // 
            // lblWarning
            // 
            this.lblWarning.AutoSize = true;
            this.lblWarning.Location = new System.Drawing.Point(391, 76);
            this.lblWarning.Name = "lblWarning";
            this.lblWarning.Size = new System.Drawing.Size(19, 13);
            this.lblWarning.TabIndex = 26;
            this.lblWarning.Text = "    ";
            // 
            // txtBxTotalLost
            // 
            this.txtBxTotalLost.Location = new System.Drawing.Point(327, 98);
            this.txtBxTotalLost.Name = "txtBxTotalLost";
            this.txtBxTotalLost.Size = new System.Drawing.Size(62, 20);
            this.txtBxTotalLost.TabIndex = 27;
            this.txtBxTotalLost.Text = "0";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(264, 101);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(57, 13);
            this.label8.TabIndex = 28;
            this.label8.Text = "Total Lost:";
            // 
            // lblRunTime
            // 
            this.lblRunTime.AutoSize = true;
            this.lblRunTime.Location = new System.Drawing.Point(395, 24);
            this.lblRunTime.Name = "lblRunTime";
            this.lblRunTime.Size = new System.Drawing.Size(19, 13);
            this.lblRunTime.TabIndex = 29;
            this.lblRunTime.Text = "    ";
            // 
            // txtBxTotalRecv
            // 
            this.txtBxTotalRecv.Location = new System.Drawing.Point(327, 124);
            this.txtBxTotalRecv.Name = "txtBxTotalRecv";
            this.txtBxTotalRecv.Size = new System.Drawing.Size(62, 20);
            this.txtBxTotalRecv.TabIndex = 30;
            this.txtBxTotalRecv.Text = "0";
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(238, 127);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(83, 13);
            this.label9.TabIndex = 31;
            this.label9.Text = "Total Received:";
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(231, 9);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(75, 13);
            this.label10.TabIndex = 32;
            this.label10.Text = "Packet viewer";
            // 
            // txtBxSeqNum
            // 
            this.txtBxSeqNum.Location = new System.Drawing.Point(133, 72);
            this.txtBxSeqNum.Name = "txtBxSeqNum";
            this.txtBxSeqNum.Size = new System.Drawing.Size(32, 20);
            this.txtBxSeqNum.TabIndex = 33;
            this.txtBxSeqNum.Text = "4";
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(16, 76);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(92, 13);
            this.label11.TabIndex = 34;
            this.label11.Text = "Seq-Num Range: ";
            // 
            // lblTotalLostPercent
            // 
            this.lblTotalLostPercent.AutoSize = true;
            this.lblTotalLostPercent.Location = new System.Drawing.Point(391, 101);
            this.lblTotalLostPercent.Name = "lblTotalLostPercent";
            this.lblTotalLostPercent.Size = new System.Drawing.Size(19, 13);
            this.lblTotalLostPercent.TabIndex = 35;
            this.lblTotalLostPercent.Text = "    ";
            // 
            // lblTotalReceivedPercent
            // 
            this.lblTotalReceivedPercent.AutoSize = true;
            this.lblTotalReceivedPercent.Location = new System.Drawing.Point(391, 127);
            this.lblTotalReceivedPercent.Name = "lblTotalReceivedPercent";
            this.lblTotalReceivedPercent.Size = new System.Drawing.Size(19, 13);
            this.lblTotalReceivedPercent.TabIndex = 36;
            this.lblTotalReceivedPercent.Text = "    ";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(914, 498);
            this.Controls.Add(this.lblTotalReceivedPercent);
            this.Controls.Add(this.lblTotalLostPercent);
            this.Controls.Add(this.label11);
            this.Controls.Add(this.txtBxSeqNum);
            this.Controls.Add(this.label10);
            this.Controls.Add(this.label9);
            this.Controls.Add(this.txtBxTotalRecv);
            this.Controls.Add(this.lblRunTime);
            this.Controls.Add(this.label8);
            this.Controls.Add(this.txtBxTotalLost);
            this.Controls.Add(this.lblWarning);
            this.Controls.Add(this.txtBxExpected);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.rTxtBxReport);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.btnClear);
            this.Controls.Add(this.btnSave);
            this.Controls.Add(this.txtBoxPacket);
            this.Controls.Add(this.txtBx_cnt);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.txtBx_TotalPacketNum);
            this.Controls.Add(this.txtBx_ReSendNum);
            this.Controls.Add(this.lbl_package);
            this.Controls.Add(this.lbl_resend);
            this.Controls.Add(this.btnClose);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.richTextBox_console);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.txtBxPrefix);
            this.Controls.Add(this.richTextBox_uart);
            this.Controls.Add(this.btnConnect);
            this.Controls.Add(this.cbSerialPort);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "Form1";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Virtual Serial Device";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.IO.Ports.SerialPort serialPort1;
        private System.Windows.Forms.ComboBox cbSerialPort;
        private System.Windows.Forms.Button btnConnect;
        private System.Windows.Forms.RichTextBox richTextBox_uart;
        private System.Windows.Forms.RichTextBox richTextBox_console;
        private System.Windows.Forms.TextBox txtBxPrefix;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Button btnClose;
        private System.Windows.Forms.Label lbl_resend;
        private System.Windows.Forms.Label lbl_package;
        private System.Windows.Forms.TextBox txtBx_ReSendNum;
        private System.Windows.Forms.TextBox txtBx_TotalPacketNum;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.TextBox txtBx_cnt;
        private System.Windows.Forms.TextBox txtBoxPacket;
        private System.Windows.Forms.Button btnSave;
        private System.Windows.Forms.Button btnClear;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.RichTextBox rTxtBxReport;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.TextBox txtBxExpected;
        private System.Windows.Forms.Label lblWarning;
        private System.Windows.Forms.TextBox txtBxTotalLost;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Label lblRunTime;
        private System.Windows.Forms.TextBox txtBxTotalRecv;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.TextBox txtBxSeqNum;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.Label lblTotalLostPercent;
        private System.Windows.Forms.Label lblTotalReceivedPercent;
    }
}

