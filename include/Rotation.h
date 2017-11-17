#ifndef UUID_054E22CC_27E3_40DC_A1AF_E25FD9D47D7A
#define UUID_054E22CC_27E3_40DC_A1AF_E25FD9D47D7A

#include <armadillo>
#include <cmath>

namespace Rotation {

// Concept Rotation (rot)
//
// A Concept of types representing 3D rotation vector operations.
// Possible implementations can use 3x3 matrices or quaternions.
//
// Member functions:
//
// rot() // uninitialized construction, the result of using the object before
//       // assigning a value to it is unspecified.
// rot(const rot&)
// rot(rot&&)
// operator=(const rot&)
// operator=(rot&&)
//
// rot(double angle, const arma::vec3& direction) // assumes normalized direction
// rot(const arma::vec3& rotvec) // angle * direction
//
// rot operator* (const rot& rhs) const // composition
// arma::vec3 operator* (const arma::vec3& v) const // apply to vector
// rot inverse()
//
// Static members:
// rot identity()

class matrix_rotation {
       private:
	arma::mat33 rot_matrix;

	explicit matrix_rotation(const arma::mat33& rot_matrix)
	    : rot_matrix(rot_matrix) {}
	explicit matrix_rotation(arma::mat33&& rot_matrix)
	    : rot_matrix(std::move(rot_matrix)) {}

       public:
	matrix_rotation() = default;  // Allow uninitialized creation, don't use
				      // before copy-assign
	matrix_rotation(double angle, const arma::vec3& direction);
	matrix_rotation(const arma::vec3& rotvec);
	static matrix_rotation identity();

	matrix_rotation operator*(const matrix_rotation& rhs) const;
	arma::vec3 operator*(const arma::vec3&)const;

	matrix_rotation inverse() const;
};

inline matrix_rotation::matrix_rotation(double angle,
					const arma::vec3& direction)
    : rot_matrix(arma::fill::eye) {
	// Rodrigues' rotation formula
	// https://en.wikipedia.org/wiki/Rodrigues%27_rotation_formula
	const auto& k = direction;
	arma::mat33 cross_matrix = {
	  { 0    , -k[2],  k[1]},
	  {  k[2],     0, -k[0]},
	  { -k[1],  k[0],     0}
	};
	rot_matrix += (
	  std::sin(angle) * arma::mat33(arma::fill::eye)
	  + (1 - std::cos(angle)) * cross_matrix
	) * cross_matrix;
}

inline matrix_rotation::matrix_rotation(const arma::vec3& rotvec)
    : matrix_rotation(arma::norm(rotvec), arma::normalise(rotvec)) {}

inline matrix_rotation matrix_rotation::identity() {
	return matrix_rotation(arma::mat33(arma::fill::eye));
}

inline matrix_rotation matrix_rotation::operator*(
    const matrix_rotation& rhs) const {
	return matrix_rotation(arma::mat33(this->rot_matrix * rhs.rot_matrix));
}

inline arma::vec3 matrix_rotation::operator*(const arma::vec3& v) const {
	return this->rot_matrix * v;
}

inline matrix_rotation matrix_rotation::inverse() const {
	return matrix_rotation(arma::mat33(this->rot_matrix.t()));
}

using rotation = matrix_rotation;

}  // namespace Rotation

#endif //  UUID_054E22CC_27E3_40DC_A1AF_E25FD9D47D7A
