#include "mathHelper.h"

glm::vec2 gx::mathHelper::rotatePoint(glm::vec2 point, glm::vec2 origin, float angle)
{
	point -= origin;
	angle = glm::radians(angle);
	glm::mat2x2 rot = { glm::cos(angle), -glm::sin(angle), glm::sin(angle), glm::cos(angle) };

	point = point * rot;
	point += origin;

	return point;
}


//https://stackoverflow.com/questions/45357715/how-to-rotate-point-around-another-one
// 3D rotation matrix rotationY = [cos(ang), 0, sin(ang), 0, 1, 0, -sin(ang), 0, cos(ang)];