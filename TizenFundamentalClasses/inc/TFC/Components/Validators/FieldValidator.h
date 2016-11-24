/*
 * FieldValidator.h
 *
 *  Created on: Nov 22, 2016
 *      Author: Kevin Winata
 */

#ifndef TFC_COMPONENTS_VALIDATORS_FIELDVALIDATOR_H_
#define TFC_COMPONENTS_VALIDATORS_FIELDVALIDATOR_H_

#include "TFC/Components/Validators/Validator.h"
#include "TFC/Components/Field.h"

namespace TFC {
namespace Components {
namespace Validators {

class FieldValidator : public Validator
{
public:
	FieldValidator(Field* field);
protected:
	Field* const field;
	int ValidateInternal() final;
	virtual int ValidateText(std::string const& str) = 0;
};

class LengthValidator : public FieldValidator
{
public:
	LengthValidator(Field* field, size_t min, size_t max);
	static int const ERROR_LENGTH_LESS;
	static int const ERROR_LENGTH_MORE;
protected:
	int ValidateText(std::string const& str);
private:
	size_t min, max;
};

class NumberValidator : public FieldValidator
{
public:
	NumberValidator(Field* field);
	static int const ERROR_NOT_NUMBER;
protected:
	int ValidateText(std::string const& str);
};

class EmailValidator : public FieldValidator
{
public:
	EmailValidator(Field* field);
	static int const ERROR_INVALID_EMAIL;
protected:
	int ValidateText(std::string const& str);
};

}
}
}



#endif /* TFC_COMPONENTS_VALIDATORS_FIELDVALIDATOR_H_ */
