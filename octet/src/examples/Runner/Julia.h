namespace octet {
	namespace shaders {
		class ColorHSV {
		public:
			ColorHSV() {}
			ColorHSV(int h, int s, int v) { _h = h; _s = s; _v = v; }
			int h() { return _h; }
			int s() { return _s; }
			int v() { return _v; }
		private:
			int _h;
			int _s;
			int _v;

		};
		class ColorRGB {
		public :
			ColorRGB() {}
			ColorRGB(int r, int g, int b) { _r = r; _b = b; _g = g; }
			int r() { return _r; }
			int g() { return _g; }
			int b() { return _b; }
		private:
			int _r;
			int _g;
			int _b;
		};
		class Julia_shader : public shader {
			GLuint modelToProjectionIndex_;
			GLuint colorIndex_;

		public:
			void init() {

				const char vertex_shader[] = SHADER_STR(
					attribute vec4 pos;
				uniform mat4 modelToProjection;

				void main() {
					gl_Position = modelToProjection * pos;
				}
				);

				const char fragment_shader[] = SHADER_STR(

					uniform vec3 color;

				void main() {

					//vec2 p = gl_FragCoord.xy / vec2(750, 750);
					//vec3 col = vec3(0.4, 0.8, 0.1) /** p.y*/;
					//gl_FragColor = color;
					gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);//color;

				}
				);

				shader::init(vertex_shader, fragment_shader);

				colorIndex_ = glGetUniformLocation(program(), "color");
				modelToProjectionIndex_ = glGetUniformLocation(program(), "modelToProjection");
			}

			ColorRGB HSVtoRGB(ColorHSV color)
			{
				int h = color.h();
				int s = color.s();
				int v = color.v();
				int r = 0;
				int g = 0;
				int b = 0;
				int i;
				int f, p, q, t;
				if (s == 0) {
					// achromatic (grey)
					r = g = b = v;
					return ColorRGB();
				}
				h /= 60;			// sector 0 to 5
				i = floor(h);
				f = h - i;			// factorial part of h
				p = v * (1 - s);
				q = v * (1 - s * f);
				t = v * (1 - s * (1 - f));
				switch (i) {
				case 0:
					r = v;
					g = t;
					b = p;
					break;
				case 1:
					r = q;
					g = v;
					b = p;
					break;
				case 2:
					r = p;
					g = v;
					b = t;
					break;
				case 3:
					r = p;
					g = q;
					b = v;
					break;
				case 4:
					r = t;
					g = p;
					b = v;
					break;
				default:		// case 5:
					r = v;
					g = p;
					b = q;
					break;
				}
				return ColorRGB(r, g, b);
			}

			void render(const mat4t &modelToProjection, const int &width, const int &height) {
				shader::render();


				//each iteration, it calculates: new = old*old + c, where c is a constant and old starts at current pixel
				double cRe, cIm;                   //real and imaginary part of the constant c, determinate shape of the Julia Set
				double newRe, newIm, oldRe, oldIm;   //real and imaginary parts of new and old
				double zoom = 1, moveX = 0, moveY = 0; //you can change these to zoom and change position
				ColorRGB color; //the RGB color value for the pixel
				int maxIterations = 300; //after how much iterations the function should stop

										 //pick some values for the constant c, this determines the shape of the Julia Set
				cRe = -0.7;
				cIm = 0.27015;

				//loop through every pixel
				for (int x = 0; x < width; x++)
					for (int y = 0; y < height; y++)
					{
						//calculate the initial real and imaginary part of z, based on the pixel location and zoom and position values
						newRe = 1.5 * (x - width / 2) / (0.5 * zoom * width) + moveX;
						newIm = (y - height / 2) / (0.5 * zoom * height) + moveY;
						//i will represent the number of iterations
						int i;
						//start the iteration process
						for (i = 0; i < maxIterations; i++)
						{
							//remember value of previous iteration
							oldRe = newRe;
							oldIm = newIm;
							//the actual iteration, the real and imaginary part are calculated
							newRe = oldRe * oldRe - oldIm * oldIm + cRe;
							newIm = 2 * oldRe * oldIm + cIm;
							//if the point is outside the circle with radius 2: stop
							if ((newRe * newRe + newIm * newIm) > 4) break;
						}
						//use color model conversion to get rainbow palette, make brightness black if maxIterations reached
						color = HSVtoRGB(ColorHSV(i % 256, 255, 255 * (i < maxIterations)));
						//draw the pixel
						glUniform3f(colorIndex_, color.r(), color.g(), color.b());
						glUniformMatrix4fv(modelToProjectionIndex_, 1, GL_FALSE, modelToProjection.get());
					}

			}
		};
	}
}
