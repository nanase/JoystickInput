using System;
using System.Windows.Forms;
using System.Drawing;

namespace JoystickSample
{
    public partial class JoystickViewer : UserControl
    {
        private readonly JoystickInput.JoystickDevice device;
        private Pen cross;

        public JoystickViewer(JoystickInput.JoystickDevice device)
        {
            InitializeComponent();

            this.device = device;

            this.cross = new Pen(Color.Red, 2f);
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            const int ButtonSize = 28;
            const int HatButtonSize = 20;
            const int CrossLength = 5;
            const int AxisSize = 148;
            const int CircleRadius = 60;

            if (this.pictureBox1.Image == null)
                this.pictureBox1.Image = new Bitmap(ButtonSize * this.device.ButtonStates.Length, ButtonSize);

            if (this.pictureBox2.Image == null)
                this.pictureBox2.Image = new Bitmap(AxisSize, AxisSize);

            if (this.pictureBox3.Image == null)
                this.pictureBox3.Image = new Bitmap(AxisSize, AxisSize);

            if (this.pictureBox4.Image == null)
                this.pictureBox4.Image = new Bitmap(AxisSize, AxisSize);

            // ボタン
            using (Graphics g = Graphics.FromImage(this.pictureBox1.Image))
            {
                g.Clear(Color.Transparent);

                for (int i = 0; i < this.device.ButtonStates.Length; i++)
                {
                    if (this.device.ButtonStates[i])
                    {
                        g.FillEllipse(Brushes.Red, i * ButtonSize, 0, ButtonSize - 1, ButtonSize - 1);
                    }

                    g.DrawEllipse(Pens.Black, i * ButtonSize, 0, ButtonSize - 1, ButtonSize - 1);
                    g.DrawString((i + 1).ToString(), this.Font, Brushes.Black, i * ButtonSize + ButtonSize / 2, ButtonSize / 2);

                }
            }

            // X-Y
            using (Graphics g = Graphics.FromImage(this.pictureBox2.Image))
            {
                g.Clear(Color.Transparent);

                float x = this.device.AxisX / 128f;
                float y = this.device.AxisY / 128f;

                float xcenter = AxisSize / 2f + (x * AxisSize / 2f);
                float ycenter = AxisSize / 2f + (y * AxisSize / 2f);

                g.DrawLine(this.cross, xcenter - CrossLength, ycenter, xcenter + CrossLength, ycenter);
                g.DrawLine(this.cross, xcenter, ycenter - CrossLength, xcenter, ycenter + CrossLength);
            }

            // Z
            using (Graphics g = Graphics.FromImage(this.pictureBox3.Image))
            {
                g.Clear(Color.Transparent);

                float x = this.device.AxisZ / 128f;
                float y = this.device.AxisRz / 128f;

                float xcenter = AxisSize / 2f + (x * AxisSize / 2f);
                float ycenter = AxisSize / 2f + (y * AxisSize / 2f);

                g.DrawLine(this.cross, xcenter - CrossLength, ycenter, xcenter + CrossLength, ycenter);
                g.DrawLine(this.cross, xcenter, ycenter - CrossLength, xcenter, ycenter + CrossLength);
            }

            // ハットスイッチ
            using (Graphics g = Graphics.FromImage(this.pictureBox4.Image))
            {
                g.Clear(Color.Transparent);

                const float xcenter = AxisSize / 2f + HatButtonSize / 2f;
                const float ycenter = AxisSize / 2f + HatButtonSize / 2f;

                if (this.device.Hat == 15)
                    g.FillEllipse(Brushes.Red, xcenter - HatButtonSize,
                                              ycenter - HatButtonSize,
                                              HatButtonSize,
                                              HatButtonSize);

                g.DrawEllipse(Pens.Black, xcenter - HatButtonSize,
                                              ycenter - HatButtonSize,
                                              HatButtonSize,
                                              HatButtonSize);

                for (int i = 0; i < 8; i++)
                {


                    if (this.device.Hat == i)
                        g.FillEllipse(Brushes.Red, xcenter + (float)Math.Sin(i * Math.PI / 4.0) * CircleRadius - HatButtonSize,
                                              ycenter - (float)Math.Cos(i * Math.PI / 4.0) * CircleRadius - HatButtonSize,
                                              HatButtonSize,
                                              HatButtonSize);

                    g.DrawEllipse(Pens.Black, xcenter + (float)Math.Sin(i * Math.PI / 4.0) * CircleRadius - HatButtonSize,
                                              ycenter - (float)Math.Cos(i * Math.PI / 4.0) * CircleRadius - HatButtonSize,
                                              HatButtonSize,
                                              HatButtonSize);
                }
            }

            this.pictureBox1.Refresh();
            this.pictureBox2.Refresh();
            this.pictureBox3.Refresh();
            this.pictureBox4.Refresh();
        }
    }
}
