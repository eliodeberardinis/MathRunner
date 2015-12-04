////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//
// Vector class
//

namespace octet { namespace math {
  class quat : public vec4
  {
  public:
	quat() {}
	quat(float x, float y, float z, float w) : vec4(x, y, z, w) {}
    quat(const vec4 &r) { *(vec4*)this = r; }
    quat operator*(const quat &r) const { return quat(qmul(r)); }
    quat operator*(float r) const { return quat((vec4&)*this * r); }
    quat &operator*=(const quat &r) { *(vec4*)this = qmul(r); return *this; }
    quat conjugate() const { return qconj(); }
    vec4 rotate(const vec4 &r) const { return (*this * r) * conjugate(); }
	quat fromAxisAngle(vec3 axis, float angle)
	{
		float x = axis.x() * sin(angle / 2);
		float y = axis.y() * sin(angle / 2);
		float z = axis.z() * sin(angle / 2);
		float w = cos(angle / 2);
		*(vec4*)this = vec4(x, y, z, w);
	}
	void toAxisAngle(vec3 &axis, float &angle)
	{
		float x, y, z;
		if (this->w() == 1.0f)
		{
			x = y = z = 0.0f;
		}
		else
		{
			x = this->x() / sqrt(1.0f - this->w());
			y = this->y() / sqrt(1.0f - this->w());
			z = this->z() / sqrt(1.0f - this->w());
		}
		float w = 2 * acos(this->w());
		axis = vec3(x, y, z);
		angle = w;
	}
  };

} }

