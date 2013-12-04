/*
 * SFAuthPlugin.cpp
 *
 *  Created on: Nov 27, 2013
 *      Author: timshi
 */

#include "SFAuthPlugin.h"
#include <bb/cascades/WebView>
#include <bb/data/JsonDataAccess>
#include "SFPluginResult.h"
#include <SFAuthenticationManager.h>
#include <SFOAuthInfo.h>
#include <QVariantMap>
#include <SFOAuthCredentials.h>

namespace sf {

static QString const kPluginAuthType = "authType";
static QString const kPluginAuthStatus = "authStatus";

static QString const kLoginDefaultSuccessCallback = "onSFOAuthFlowSuccess";
static QString const kLoginDefaultFailCallback = "onSFOAuthFlowFailure";
static QString const kLoginDefaultCancelCallback = "onSFOAuthFlowCancel";
static QString const kLogoutDefaultSuccessCallback = "onSFUserLoggedOut";

/*
SFAuthPlugin::SFAuthPlugin() {
	mWebView = NULL;
}
*/

SFAuthPlugin::SFAuthPlugin(WebView* webView):SFPlugin(webView){
	//mWebView = webView;
    //connect slots for authentication
	SFAuthenticationManager* authManager = SFAuthenticationManager::instance();
	//connect slots
	connect(authManager, SIGNAL(SFOAuthFlowSuccess(SFOAuthInfo*)), this, SLOT(onSFOAuthFlowSuccess(SFOAuthInfo*)), Qt::UniqueConnection);
	connect(authManager, SIGNAL(SFOAuthFlowCanceled(SFOAuthInfo*)), this, SLOT(onSFOAuthFlowCanceled(SFOAuthInfo*)), Qt::UniqueConnection);
	connect(authManager, SIGNAL(SFOAuthFlowFailure(SFOAuthInfo*)), this, SLOT(onSFOAuthFlowFailure(SFOAuthInfo*)), Qt::UniqueConnection);
	connect(authManager, SIGNAL(SFUserLoggedOut()), this, SLOT(onSFUserLoggedOut()), Qt::UniqueConnection);
}

SFAuthPlugin::~SFAuthPlugin() {
}

void SFAuthPlugin::login(const SFInvokedUrlCommand& command){
	qDebug()<<"login called with command callbackId" << command.getCallbackId();
	callbackMap.insert(command.getMethodName(), command.getCallbackId());
	SFAuthenticationManager::instance()->login();
}

void SFAuthPlugin::logout(const SFInvokedUrlCommand& command){
	qDebug()<<"logout called with command callbackId" << command.getCallbackId();
	callbackMap.insert(command.getMethodName(), command.getCallbackId());
	SFAuthenticationManager::instance()->logout();
}

void SFAuthPlugin::getCredentials(const SFInvokedUrlCommand& command){
	const SFOAuthCredentials* credentials = SFAuthenticationManager::instance()->getCredentials();
	QVariantMap credentialsMap;
	const QMetaObject *metaObj = credentials->metaObject();
	int count = metaObj->propertyCount();
	for(int i=0; i<count; i++) {
		QMetaProperty property = metaObj->property(i);
		if (!property.isReadable()) {
			continue;
		}
		credentialsMap.insert(property.name(),property.read(credentials));
	}
	credentialsMap.insert("accessToken", credentials->getAccessToken());
	SFPluginResult result = SFPluginResult(SFCommandStatus_OK, credentialsMap, false);
	this->sendPluginResult(result,command.getCallbackId());
}

/*
 * slots
 */
void SFAuthPlugin::onSFOAuthFlowSuccess(SFOAuthInfo* info){
	QString callbackId = callbackMap.value("login"); //matching method name
	QVariantMap message;
	message.insert(kPluginAuthType, info->getAuthType());
	message.insert(kPluginAuthStatus, SFAuth_SUCCESS);
	SFPluginResult result = SFPluginResult(SFCommandStatus_OK, message, false);
	if (!callbackId.isEmpty() && !callbackId.isNull()){
		this->sendPluginResult(result,callbackId);
		callbackMap.remove("login");
	}else{
		QString js = QString("%1(%2)").arg(kLoginDefaultSuccessCallback, variantToJSON(message));
		this->evalJavascript(js);
	}
}
void SFAuthPlugin::onSFOAuthFlowFailure(SFOAuthInfo* info){
	QString callbackId = callbackMap.value("login"); //matching method name
	QVariantMap message;
	message.insert(kPluginAuthType, info->getAuthType());
	message.insert(kPluginAuthStatus, SFAuth_FAIL);
	SFPluginResult result = SFPluginResult(SFCommandStatus_OK, message, false);
	if (!callbackId.isEmpty() && !callbackId.isNull()){
		this->sendPluginResult(result,callbackId);
		callbackMap.remove("login");
	}else{
		QString js = QString("%1(%2)").arg(kLoginDefaultFailCallback, variantToJSON(message));
		this->evalJavascript(js);
	}
}
void SFAuthPlugin::onSFOAuthFlowCanceled(SFOAuthInfo* info){
	QString callbackId = callbackMap.value("login"); //matching method name
	QVariantMap message;
	message.insert(kPluginAuthType, info->getAuthType());
	message.insert(kPluginAuthStatus, SFAuth_CANCEL);
	SFPluginResult result = SFPluginResult(SFCommandStatus_OK, message, false);
	if (!callbackId.isEmpty() && !callbackId.isNull()){
		this->sendPluginResult(result,callbackId);
		callbackMap.remove("login");
	}else{
		QString js = QString("%1(%2)").arg(kLoginDefaultCancelCallback, variantToJSON(message));
		this->evalJavascript(js);
	}
}

void SFAuthPlugin::onSFUserLoggedOut(){
	qDebug()<<"on logged out";
	QString callbackId = callbackMap.value("logout"); //matching method name
	QVariantMap message;
	SFPluginResult result = SFPluginResult(SFCommandStatus_OK, message, false);
	qDebug()<<"send logged out result";
	if (!callbackId.isEmpty() && !callbackId.isNull()){
		this->sendPluginResult(result,callbackId);
		callbackMap.remove("logout");
	}else{
		QString js = QString("%1()").arg(kLogoutDefaultSuccessCallback);
		this->evalJavascript(js);
	}
}
}/* namespace sf */