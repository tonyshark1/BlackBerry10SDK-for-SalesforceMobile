/*
* Copyright 2013 BlackBerry Limited.
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
* SFRestAPI.h
*
*  Created on: Oct 18, 2013
*      Author: Livan Yi Du
*/

#ifndef SFRESTAPI_H_
#define SFRESTAPI_H_

#include <QObject>
#include <QVariant>
#include <QStringList>
#include <QQueue>
#include "SFGlobal.h"
#include "SFRestRequest.h"
#include "SFResult.h"

class QScriptValue;

namespace sf {
class SFOAuthCredentials;
class SFAccountManager;
class SFGenericTask;
class SFRestResourceTask;
class SFOAuthInfo;

/*!
 * @class SFRestAPI
 * @headerfile SFRestAPI.h <rest/SFRestAPI.h>
 * @brief The SFRestAPI is the main class to send REST requests to Force.com REST API
 *
 * @details
 * The class is singleton and the global instance can be accessed by @c SFRestAPI::instance()
 *
 * Initialization
 * ---------------------
 * Before using the class, some minimum configuration is required:
 * - All required configurations for Force.com authentication should be set prior to use the class. The required configurations is documented in @c SFAuthenticationManager
 * - The force.com API version should be set via @c SFRestAPI::apiVersion
 * - @c SFRestAPI::endPoint and @c SFRestAPI::userAgent is automatically set. If custom values are assigned, it will affect all REST request generated by this class.
 *
 * QML and C++ Support
 * -------------------
 * This class is designed to be accessible from both C++ and QML. Some APIs contains different function signatures for C++ and QML. To use this class in QML, additional setup is required:
 * - In the constructor of your main application class, add following code to register necessary meta types and assign context property:
 * @code{.cpp}
 * #include "SFGlobal.h"
 * MyApplicationUI::MyApplicationUI(bb::cascades::Application *app) : QObject(app) {
 * 		sfRegisterMetaTypes();
 * 		myDeclarativeContext->setContextProperty("SFRestAPI", SFRestAPI::instance());
 * }
 * @endcode
 *
 * Convenient Setup
 * ----------------
 * The alternative way of preparing the API instance is to use @c SFAbstractApplicationUI as the base class of your main application class and define required global variables.
 *
 *
 * Usage
 * -----
 * Example: send REST request in QML
 * \code{.qml}
 * Button {
 * 	id: button
 * 	text: "Send REST Request(QML)"
 * 		onClicked: {
 * 			var request = SFRestAPI.requestForDescribeObject("Account");
 * 			SFRestAPI.sendRestRequest(request, button, button.onTaskResultReady)
 * 		}
 *
 * 		function onTaskResultReady(result) {
 * 			//Handle result as SFResult
 * 		}
 * }
 * @endcode
 *
 * Example: send REST request in C++
 * @code{.cpp}
 * 		QVariantMap fields;
 * 		fields["Name"] = "My Account Name";
 * 		fields["BillingCity"] = "Toronto";
 * 		SFRestRequest * request = SFRestAPI::instance()->requestForUpdateObject("Account", "001E000000kJbPTIA0", fields);
 * 		SFRestAPI::instance()->sendRestRequest(request, this, SLOT(onUpdateResultReady(sf::SFResult*)));
 * @endcode
 *
 * \sa SFAuthenticationManager, SFAbstractApplicationUI, SFRestRequest, SFResult, SFRestResourceTask
 *
 * See the [Force.com REST API Developer's Guide](http://www.salesforce.com/us/developer/docs/api_rest/index.htm) for more information regarding the Force.com REST API.
 *
 * \author Livan Yi Du
 */
class SFRestAPI : public QObject {
	Q_OBJECT
	Q_PROPERTY(QString endPoint READ endPoint WRITE setEndPoint) /*!< Force.com REST service end point. The default value is @c DefaultEndPoint */
	Q_PROPERTY(QString apiVersion READ apiVersion WRITE setApiVersion) /*!< Force.com REST API version. Example: "/v28.0 The default value is empty string*/
	Q_PROPERTY(QString userAgent READ userAgent WRITE setUserAgent) /*!< User agent string used for all HTTP/HTTPS requests */

public:
	virtual ~SFRestAPI();

	Q_INVOKABLE static SFRestAPI *instance(); /*!< @return The singleton instance of SFRestAPI */

	/* convenient accessors */
	const SFOAuthCredentials* currentCredentials(); /*!< @return The SFOAuthCredentials object of current salesforce session. */

	/* setter and getter */
	/*! @return Force.com API end point */
	const QString & endPoint() const { return this->mEndPoint;};
	/*! @return Force.com API version string */
	const QString & apiVersion() const { return this->mApiVersion;};
	/*! @return User-Agent string for HTTP/HTTPS requests */
	const QString & userAgent() const { return this->mUserAgent;};
	/*! @param endPoint String of Force.com API end point */
	void setEndPoint(const QString & endPoint) { this->mEndPoint = endPoint;};
	/*! @param apiVersion String of Force.com API version */
	void setApiVersion(const QString & apiVersion) { this->mApiVersion = apiVersion;};
	/*! @param userAgent String of HTTP User-Agent */
	void setUserAgent(const QString & userAgent) { this->mUserAgent = userAgent;};

	/* REST APIs */
	/*! Send REST request asynchronously. The result will be delivered to @a resultReciever as an instance of @c SFResult.
	 *
	 * The @a tag is an arbitrary value that will be carried on into @c SFResult and is accessible using @c SFResult::tags() with global variable @c ::kSFRestRequestTag as key;
	 *
	 * @remark This function takes ownership of the @a request. The @a request will be automatically deleted after the @a resultRecieverSlot exit.
	 * @remark This function is designed for C++. QML should call the corresponding QML version API.
	 * @param request A pointer to a instance of @c SFRestRequest.
	 * @param resultReciever The result receiver QObject.
	 * @param resultRecieverSlot The method in receiver object. You must use @c SLOT() macro. The slot should take one parameter with type of @c SFResult*.
	 * @param tag An arbitrary value that you want to pass on to the receiver slot.
	 * @sa SFRestRequest, SFResult, SFRestAPI::sendRestRequest(sf::SFRestRequest*,const QScriptValue&,const QScriptValue&,const QVariant&)
	 */
	void sendRestRequest(SFRestRequest * request, QObject * resultReciever = NULL, const char * resultRecieverSlot = NULL, const QVariant & tag = QVariant());

	/*! This is the QML version of the same API.
	 *
	 * @param request A pointer to a instance of @c SFRestRequest.
	 * @param resultReciever A QScriptValue containing a pointer to the receiver object
	 * @param resultRecieverSlot A QScriptValue containing a script function. The function should take one parameter.
	 * @param tag An arbitrary value that you want to pass on to the receiver slot.
	 * @sa SFRestRequest, SFResult, SFRestAPI::sendRestRequest(SFRestRequest*,QObject*,const char*,const QVariant &)
	 */
	Q_INVOKABLE void sendRestRequest(sf::SFRestRequest * request, const QScriptValue & resultReciever, const QScriptValue & resultRecieverSlot, const QVariant & tag = QVariant());

	/*! Creates a @c SFRestRequest which lists summary information about each Salesforce.com version currently available.
	 * @see http://www.salesforce.com/us/developer/docs/api_rest/Content/resources_versions.htm
	 * @remark The returned object has parent set to 0. If the method is called in C++, it's your responsibility to manage the memory. If the method is called from QML, the ownership is automatically transfered to the JavaScript engine.
	 * @return the pointer to the created SFRestRequest. */
	Q_INVOKABLE sf::SFRestRequest * requestForVersions();

	/*! Creates a @c SFRestRequest which lists available resources for the current configured API version, including resource name and URI.
	 * @see http://www.salesforce.com/us/developer/docs/api_rest/Content/resources_discoveryresource.htm
	 * @remark The returned object has parent set to 0. If the method is called in C++, it's your responsibility to manage the memory. If the method is called from QML, the ownership is automatically transfered to the JavaScript engine.
	 * @return the pointer to the created SFRestRequest. */
	Q_INVOKABLE sf::SFRestRequest * requestForResources();

	/*! Creates a @c SFRestRequest which lists the available objects and their metadata for your organization's data.
	 * @see http://www.salesforce.com/us/developer/docs/api_rest/Content/resources_describeGlobal.htm
	 * @remark The returned object has parent set to 0. If the method is called in C++, it's your responsibility to manage the memory. If the method is called from QML, the ownership is automatically transfered to the JavaScript engine.
	 * @return the pointer to the created SFRestRequest. */
	Q_INVOKABLE sf::SFRestRequest * requestForDescribeGlobal();

	/*! Creates a @c SFRestRequest which lists the summary of the individual metadata for the specified object.
	 * @see http://www.salesforce.com/us/developer/docs/api_rest/Content/resources_sobject_basic_info.htm
	 * @remark The returned object has parent set to 0. If the method is called in C++, it's your responsibility to manage the memory. If the method is called from QML, the ownership is automatically transfered to the JavaScript engine.
	 * @param objectType String of the object type. Example: "Account"
	 * @return the pointer to the created SFRestRequest. */
	Q_INVOKABLE sf::SFRestRequest * requestForMetadata(const QString & objectType);

	/*! Creates a @c SFRestRequest which completely describes the individual metadata for the sepcified object.
	 * @see http://www.salesforce.com/us/developer/docs/api_rest/Content/resources_sobject_describe.htm
	 * @remark The returned object has parent set to 0. If the method is called in C++, it's your responsibility to manage the memory. If the method is called from QML, the ownership is automatically transfered to the JavaScript engine.
	 * @param objectType String of the object type. Example: "Account"
	 * @return the pointer to the created SFRestRequest. */
	Q_INVOKABLE sf::SFRestRequest * requestForDescribeObject(const QString & objectType);

	/*! Creates a @c SFRestRequest which retrieves field values for a record of the given type.
	 * @see http://www.salesforce.com/us/developer/docs/api_rest/Content/resources_sobject_retrieve.htm
	 * @remark The returned object has parent set to 0. If the method is called in C++, it's your responsibility to manage the memory. If the method is called from QML, the ownership is automatically transfered to the JavaScript engine.
	 * @param objectType String of the object type. Example: "Account"
	 * @param objectId The object's object ID.
	 * @param fieldList The list of fields for which to return values. Example: ["Name", "BillingCity", "CustomField__c"]
	 * @return the pointer to the created SFRestRequest. */
	Q_INVOKABLE sf::SFRestRequest * requestForRetrieveObject(const QString & objectType, const QString & objectId, const QStringList & fieldList);

	/*! Creates a @c SFRestRequest which creates a new record of the given type.
	 * @see http://www.salesforce.com/us/developer/docs/api_rest/Content/dome_sobject_create.htm
	 * @remark The returned object has parent set to 0. If the method is called in C++, it's your responsibility to manage the memory. If the method is called from QML, the ownership is automatically transfered to the JavaScript engine.
	 * @param objectType String of the object type. Example: "Account"
	 * @param fields A QVariantMap containing initial field names and values for the record.
	 * For example, {Name: "salesforce.com", TickerSymbol: "CRM"}
	 * @return the pointer to the created SFRestRequest. */
	Q_INVOKABLE sf::SFRestRequest * requestForCreateObject(const QString & objectType, const QVariantMap & fields);

	/*! Creates a @c SFRestRequest which updates field values on a record of the given type.
	 * @see http://www.salesforce.com/us/developer/docs/api_rest/Content/dome_update_fields.htm
	 * @remark The returned object has parent set to 0. If the method is called in C++, it's your responsibility to manage the memory. If the method is called from QML, the ownership is automatically transfered to the JavaScript engine.
	 * @param objectType String of the object type. Example: "Account"
	 * @param objectId The object's object ID.
	 * @param fields A QVariantMap containing initial field names and values for the record.
	 * For example, {Name: "salesforce.com", TickerSymbol: "CRM"}
	 * @return the pointer to the created SFRestRequest. */
	Q_INVOKABLE sf::SFRestRequest * requestForUpdateObject(const QString & objectType, const QString & objectId, const QVariantMap & fields);

	/*! Creates a @c SFRestRequest which creates or updates record of the given type, based on the given external ID.
	 * @see http://www.salesforce.com/us/developer/docs/api_rest/Content/resources_sobject_upsert.htm
	 * @remark The returned object has parent set to 0. If the method is called in C++, it's your responsibility to manage the memory. If the method is called from QML, the ownership is automatically transfered to the JavaScript engine.
	 * @param objectType String of the object type. Example: "Account"
	 * @param extIdField external ID field name. For example, "accountMaster__c"
	 * @param extIdValue the record's external ID value
	 * @param fields A QVariantMap containing initial field names and values for the record.
	 * For example, {Name: "salesforce.com", TickerSymbol: "CRM"}
	 * @return the pointer to the created SFRestRequest. */
	Q_INVOKABLE sf::SFRestRequest * requestForUpsertObject(const QString & objectType, const QString & extIdField, const QString & extIdValue, const QVariantMap & fields);

	/*! Creates a @c SFRestRequest which deletes a record of the given type.
	 * @see http://www.salesforce.com/us/developer/docs/api_rest/Content/dome_delete_record.htm
	 * @remark The returned object has parent set to 0. If the method is called in C++, it's your responsibility to manage the memory. If the method is called from QML, the ownership is automatically transfered to the JavaScript engine.
	 * @param objectType String of the object type. Example: "Account"
	 * @param objectId The object's object ID.
	 * @return the pointer to the created SFRestRequest. */
	Q_INVOKABLE sf::SFRestRequest * requestForDeleteObject(const QString & objectType, const QString & objectId);

	/*! Creates a @c SFRestRequest which executes the specified SOQL query.
	 * @see http://www.salesforce.com/us/developer/docs/api_rest/Content/resources_query.htm
	 * @remark The returned object has parent set to 0. If the method is called in C++, it's your responsibility to manage the memory. If the method is called from QML, the ownership is automatically transfered to the JavaScript engine.
	 * @param soql A string containing the query to execute. For example, "SELECT Id,Name from Account ORDER BY Name LIMIT 20"
	 * @return the pointer to the created SFRestRequest. */
	Q_INVOKABLE sf::SFRestRequest * requestForQuery(const QString & soql);

	/*! Creates a @c SFRestRequest which executes the specified SOSL search.
	 * @see http://www.salesforce.com/us/developer/docs/api_rest/Content/resources_search.htm
	 * @remark The returned object has parent set to 0. If the method is called in C++, it's your responsibility to manage the memory. If the method is called from QML, the ownership is automatically transfered to the JavaScript engine.
	 * @param sosl a string containing the search to execute. For example, "FIND {needle}"
	 * @return the pointer to the created SFRestRequest. */
	Q_INVOKABLE sf::SFRestRequest * requestForSearch(const QString & sosl);

	/*
	 * NOTE: Because QScriptEngine is not aware of enums, it cannot correctly interpret the signature.
	 * 		Therefore this function should be only used in C++
	 */
	/*! Creates a custom @c SFRestRequest. This function ignores values in @c SFRestAPI::endPoint and @c SFRestAPI::apiVersion.
	 * @note Because QScriptEngine is not aware of enums, it cannot correctly interpret the signature.
	 * Therefore this function should be only used in C++.
	 * @remark The returned object has parent set to 0. If the method is called in C++, it's your responsibility to manage the memory. If the method is called from QML, the ownership is automatically transfered to the JavaScript engine.
	 * @param path Relative path to your application's instance URL, or absolute path of your HTTP URL. If you use absolute path, you need to include
	 * the scheme.
	 * @param method HTTP verb, GET, POST, HEAD, DELETE, etc. Please see @c HTTPMethod::Type for more details.
	 * @param params The key-value pairs you want included in your REST request. You don't need to escape HTTP reserved characters.
	 * @param contentType Indicate how to encode key-value pairs stored in @a params. If your HTTP verb are GET, HEAD or DELETE
	 * This the contentType is should be @c SFRestRequest::HTTPContentTypeUrlEncoded. Please see @c SFRestRequest::HTTPContentType for more details.
	 * @note SFRestRequest::HTTPContentTypeMultiPart is not supported in SDK version <em>1.0</em>.
	 * @return the pointer to the created SFRestRequest.
	 * @sa SFRestAPI::customRequest(const QString&,const QScriptValue&,const QVariantMap &,const QScriptValue&),
	 * HTTPMethod::Type,
	 * SFRestRequest::HTTPContentType*/
	Q_INVOKABLE sf::SFRestRequest * customRequest(
			const QString & path,
			const HTTPMethodType & method = HTTPMethod::HTTPGet,
			const QVariantMap & params = QVariantMap(),
			const sf::SFRestRequest::HTTPContentType & contentType = sf::SFRestRequest::HTTPContentTypeJSON);

	/* NOTE: This is the corresponding QML version of API, it internally calls the C++ version to create the request */
	/*! Creates a custom @c SFRestRequest. This is the corresponding QML version of the API.
	 * @remark The returned object has parent set to 0. If the method is called in C++, it's your responsibility to manage the memory. If the method is called from QML, the ownership is automatically transfered to the JavaScript engine.
	 * @param path Relative path to your application's instance URL, or absolute path of your HTTP URL. If you use absolute path, you need to include
	 * the scheme.
	 * @param method A @c QScriptValue containing an integer that matches the values of @c HTTPMethod::Type.
	 * @param params The key-value pairs you want included in your REST request. You don't need to escape HTTP reserved characters.
	 * @param contentType A @c QScriptValue containing an integer that matches the values of @c SFRestRequest::HTTPContentType.
	 * @note SFRestRequest::HTTPContentTypeMultiPart is not supported in SDK version <em>1.0</em>.
	 * @return the pointer to the created SFRestRequest.
	 * @sa SFRestAPI::customRequest(const QString&,const HTTPMethodType&,const QVariantMap &,const sf::SFRestRequest::HTTPContentType &),
	 * HTTPMethod::Type,
	 * SFRestRequest::HTTPContentType*/
	Q_INVOKABLE sf::SFRestRequest * customRequest(
				const QString & path,
				const QScriptValue & method,
				const QVariantMap & params,
				const QScriptValue & contentType);

	/* Convenience APIs */
	/*! Generate a SOQL query.
	 * @param fields - @c QStringList of fields to select. e.g. ["Id", "CustomField__c"]
	 * @param sObject - object type to query. e.g. "Account"
	 * @param where - WHERE clause. e.g. "Name='Salesforce'"
	 * @param groupBy - @c QStringList of GROUP BY fields
	 * @param having - HAVING clause
	 * @param orderBy - @c QStringList of ORDER BY fields
	 * @param limit - number for LIMIT clause. The LIMIT clause is not added if not added if give 0
	 * @return Generated SOQL query */
	Q_INVOKABLE QString generateSOQLQuery(const QStringList & fields, const QString & sObjectType,
			const QString & where = "",
			const QStringList & groupBy = QStringList(),
			const QString & having = "",
			const QStringList & orderBy = QStringList(),
			const int & limit = 0);

	/*!
	 * Generate a SOSL search string.
	 * @param term - the search term. e.g. "John*"
	 * @param fieldScope - the SOSL scope, e.g. "IN ALL FIELDS". If empty, defaults to "IN NAME FIELDS"
	 * @param objectScope - a @c QVariantMap where each key is an sObject name and each value is a string
	 * with the fieldlist and (optional) where, order by, and limit clause for that object.
	 * Set empty to search all searchable objects.
	 * @param limit - overall search limit (max 200)
	 * @return Generated SOSL search string */
	Q_INVOKABLE QString generateSOSLSearchQuery(const QString & searchTerm,
			const QString & fieldScope = "",
			const QVariantMap & objectScope = QVariantMap(),
			const int & limit = 0);

	/*! @param searchTerm The search term to be sanitized.
	 * @return SOSL-safe version of search term */
	Q_INVOKABLE QString sanitizeSOSLSearchTerm(const QString & searchTerm);

	//More APIs

private:
	/* private constructor, Singleton */
	SFRestAPI();

	static SFRestAPI *sharedInstance;
	QString mEndPoint;
	QString mApiVersion;
	QString mUserAgent;

	QQueue<SFRestResourceTask*> mPendingTasks;

	SFRestResourceTask* createRestTask(SFRestRequest * request, const QVariant & tag = QVariant());
	void startRestTask(SFRestResourceTask * task);
	void resendAllPendingTasks();

	QString findWebKitUserAgent();
	QString constructUserAgent();

private slots:
	void onSFOAuthFlowSuccess(SFOAuthInfo*);
	void onSFOAuthFlowFailure(SFOAuthInfo*);
	void onSFOAuthFlowCanceled(SFOAuthInfo*);

	void onTaskShouldRetry(sf::SFGenericTask*, sf::SFResult*);
};

} /* namespace sf */
#endif /* SFRESTAPI_H_ */
