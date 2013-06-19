using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;

namespace JoystickSample
{
    public partial class Form1 : Form
    {
        private readonly JoystickInput.Joystick joystick;
        private readonly List<JoystickInput.JoystickDevice> devices;

        public Form1()
        {
            InitializeComponent();

            this.joystick = new JoystickInput.Joystick(this.Handle);
            this.devices = new List<JoystickInput.JoystickDevice>();
        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }

        protected override void WndProc(ref Message m)
        {
            switch (m.Msg)
            {
                case 0x00FF:    // WM_INPUT
                    joystick.Update(m);

                    foreach (var newdevice in joystick.Devices.Where(d => !this.devices.Contains(d)))
                    {
                        this.flowLayoutPanel1.Controls.Add(new JoystickViewer(newdevice));
                        this.devices.Add(newdevice);
                    }
                    break;

                default:
                    break;
            }

            base.WndProc(ref m);
        }
    }
}
