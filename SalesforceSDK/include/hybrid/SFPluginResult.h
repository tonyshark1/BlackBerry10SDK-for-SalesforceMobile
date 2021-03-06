/*
 * Copyright (c) 2011-2013 BlackBerry Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * SFPluginResult.h
 *
 *  Created on: Nov 27, 2013
 *      Author: timshi
 */

#ifndef SFPLUGINRESULT_H_
#define SFPLUGINRESULT_H_

#include <QVariant>

namespace sf {

typedef enum {
    SFCommandStatus_NO_RESULT = 0,
    SFCommandStatus_OK,
    SFCommandStatus_CLASS_NOT_FOUND_EXCEPTION,
    SFCommandStatus_ILLEGAL_ACCESS_EXCEPTION,
    SFCommandStatus_INSTANTIATION_EXCEPTION,
    SFCommandStatus_MALFORMED_URL_EXCEPTION,
    SFCommandStatus_IO_EXCEPTION,
    SFCommandStatus_INVALID_ACTION,
    SFCommandStatus_JSON_EXCEPTION,
    SFCommandStatus_ERROR
} SFPluginCommandStatus;

/*!
 * @class SFPluginResult
 * @headerfile SFPluginResult <hybrid/SFPluginResult.h>
 * @brief The SFPluginResult encapsulates the information that a plugin needs to pass back to the javascript when the native portion of the plugin execution completes
 *
 * @details
 * When a plugin finishes executing its native codes, it encapsulates the result in an instance of this class, and post it to javascript by calling the sendPluginResult method on @c SFPlugin
 */
class SFPluginResult {
public:
	/*!
	 * Creates a result instance
	 * @param status The plugin execution status.
	 * @param message The message to be passed back to the javascript call back function.
	 * @param keepCallback A flag to let the javascript know whether there's more result coming back. Based on this the javascript side will keep or delete the callback function after the call back function is evaluated.
	 */
	SFPluginResult(SFPluginCommandStatus status, const QVariant& message, bool keepCallback);
	virtual ~SFPluginResult();
	/*!
	 * @return Whether the javascript callback function needs to be kept (if more result is anticipated to be sent back)
	 */
	bool isKeepCallback() const {return mKeepCallback;}
	/*!
	 * @return The message for the javascript callback function
	 */
	const QVariant& getMessage() const {return mMessage;}
	/*!
	 * @return The status of the plugin execution. Based on this value, either the success or failure call back will be evaluated in javascript
	 */
	SFPluginCommandStatus getStatus() const {return mStatus;}

private:
	SFPluginResult();
	SFPluginCommandStatus mStatus;
	QVariant mMessage;
	bool mKeepCallback;
};

} /* namespace sf */
#endif /* SFPLUGINRESULT_H_ */
