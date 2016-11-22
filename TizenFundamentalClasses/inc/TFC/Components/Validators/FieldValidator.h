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
protected:
	Field* const field;
	bool ValidateInternal() final;
	virtual bool ValidateText(std::string const& str) = 0;
public:
	FieldValidator(Field* field);
};

class LengthValidator : public FieldValidator
{
private:
	size_t min, max;
protected:
	bool ValidateText(std::string const& str);
public:
	LengthValidator(Field* field, size_t min, size_t max);
};

class NumberValidator : public FieldValidator
{
protected:
	bool ValidateText(std::string const& str);
public:
	NumberValidator(Field* field);
};

class EmailValidator : public FieldValidator
{
protected:
	bool ValidateText(std::string const& str);
public:
	EmailValidator(Field* field);
};

}
}
}



#endif /* TFC_COMPONENTS_VALIDATORS_FIELDVALIDATOR_H_ */
