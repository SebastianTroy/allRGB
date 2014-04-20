import tCode.TCode;

public class Main extends TCode
	{
		public Main(int width, int height, boolean framed, boolean resizable)
			{
				super(900, 700, framed, resizable);

				DEBUG = true;
				FORCE_SINGLE_THREAD = true;

				programName = "All RGB picture approximator";
				versionNumber = "1.1";

				begin(new PictureApproximator());
			}

		public static void main(String[] args)
			{
				new Main(900, 600, true, true);
			}
	}