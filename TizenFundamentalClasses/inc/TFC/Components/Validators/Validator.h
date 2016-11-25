/*
 * Validator.h
 *
 *  Created on: Nov 22, 2016
 *      Author: Kevin Winata
 */

#ifndef TFC_COMPONENTS_VALIDATORS_VALIDATOR_H_
#define TFC_COMPONENTS_VALIDATORS_VALIDATOR_H_

#include "TFC/Components/ComponentBase.h"
#include <vector>
#include <unordered_map>
#include <regex>
#include <functional>

namespace TFC {
namespace Components {
namespace Validators {

class Validator {
public:
	Validator(ComponentBase* component);
	virtual ~Validator();
	int Validate();
	std::string const& ErrorMessage;

	static int const ERROR_NONE;
	static int const ERROR_INVALID_COMPONENT;
	void SetErrorMessageFormat(int error, std::string const& message);
protected:
	ComponentBase* const component;
	int validationResult;
	virtual int ValidateInternal() = 0;

	std::unordered_map<int, std::string> errorDictionary;
	std::vector<std::function<char const*()>> formatFunctions;
private:
	std::string errorMessage;
	void GenerateErrorMessage();
};

class ValidatorList {
public:
	ValidatorList();
	~ValidatorList();
	void AddValidator(Validator* validator);
	bool ValidateAll();
	const std::string& LastValidationMessage;
private:
	std::vector<Validator*> validators;
	int lastValidationResult;
	std::string lastValidationMessage;
};

}
}
}



#endif /* TFC_COMPONENTS_VALIDATORS_VALIDATOR_H_ */
