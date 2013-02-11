//===========================================================================
/*!
 *
 *  \brief unit test for the radius margin quotient
 *
 *
 *  \author  T. Glasmachers
 *  \date    2011
 *
 *  \par Copyright (c) 2011:
 *      Institut f&uuml;r Neuroinformatik<BR>
 *      Ruhr-Universit&auml;t Bochum<BR>
 *      D-44780 Bochum, Germany<BR>
 *      Phone: +49-234-32-25558<BR>
 *      Fax:   +49-234-32-14209<BR>
 *      eMail: Shark-admin@neuroinformatik.ruhr-uni-bochum.de<BR>
 *      www:   http://www.neuroinformatik.ruhr-uni-bochum.de<BR>
 *
 *
 *  <BR><HR>
 *  This file is part of Shark. This library is free software;
 *  you can redistribute it and/or modify it under the terms of the
 *  GNU General Public License as published by the Free Software
 *  Foundation; either version 3, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this library; if not, see <http://www.gnu.org/licenses/>.
 *
 */
//===========================================================================

#include <shark/ObjectiveFunctions/RadiusMarginQuotient.h>
#include <shark/Models/Kernels/GaussianRbfKernel.h>

#define BOOST_TEST_MODULE ObjectiveFunctions_RadiusMarginQuotient
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

using namespace shark;




BOOST_AUTO_TEST_CASE( ObjectiveFunctions_RadiusMarginQuotient )
{
	std::vector<RealVector> inputs(4, RealVector(1));
	inputs[0](0) = 0.0;
	inputs[1](0) = 0.1;
	inputs[2](0) = 0.2;
	inputs[3](0) = 0.3;
	std::vector<unsigned int> targets(4);
	targets[0] = 0;
	targets[1] = 0;
	targets[2] = 1;
	targets[3] = 1;

	ClassificationDataset dataset = createLabeledDataFromRange(inputs, targets);
	RealVector parameters(1, 0.5);
	GaussianRbfKernel<> kernel;
	kernel.setParameterVector(parameters);
	RadiusMarginQuotient<RealVector> rm(dataset, &kernel);

	// check the value of the objective function
	double radius_squared = kernel.featureDistanceSqr(dataset.element(0).input, dataset.element(3).input) / 4.0;
	double margin_squared = kernel.featureDistanceSqr(dataset.element(1).input, dataset.element(2).input) / 4.0;
	double should = radius_squared / margin_squared;
	double quotient = rm.eval(parameters);
	BOOST_CHECK_SMALL(quotient - should, 0.01);

	// numerically check the derivative
	double delta = 0.001;
	parameters(0) += delta;
	double right = rm.eval(parameters);
	parameters(0) -= 2 * delta;
	double left = rm.eval(parameters);
	double estimate = (right - left) / (2 * delta);
	RadiusMarginQuotient<RealVector>::FirstOrderDerivative derivative;
	rm.evalDerivative(parameters, derivative);
	BOOST_CHECK_SMALL(derivative.m_gradient(0) - estimate, 0.01);
}
