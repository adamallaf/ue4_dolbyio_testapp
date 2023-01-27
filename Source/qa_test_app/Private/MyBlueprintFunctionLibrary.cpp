// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBlueprintFunctionLibrary.h"
#include "Runtime/Online/HTTP/Public/Interfaces/IHttpResponse.h"
#include "Runtime/Core/Public/GenericPlatform/GenericPlatformProcess.h"



bool UMyBlueprintFunctionLibrary::getVoxeetToken(const FString& url, FString& token) {
	FThreadSafeSharedStringPtr str;
	FHttpRequestPtr request = FHttpModule::Get().CreateRequest();
	request->SetURL(url);
	request->SetHeader("accept", "application/json, text/plain, */*");
	request->OnProcessRequestComplete().BindLambda([&token](FHttpRequestPtr _request, FHttpResponsePtr _response, bool success) {
		if ( success ) {
			int32 content_length = _response->GetContentLength();
			FString content = _response->GetContentAsString();
			UE_LOG(LogTemp, Display, TEXT("received data [ %d Bytes ]:\n%s"), content_length, *content);
			TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
			TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(content);

			if (FJsonSerializer::Deserialize(JsonReader, JsonObject)) {
				if (JsonObject.IsValid()) {
					token = JsonObject->GetStringField("access_token");
				}
				else {
					UE_LOG(LogTemp, Warning, TEXT("received data is not a JSON"));
				}
			}
			else {
				UE_LOG(LogTemp, Error, TEXT("could not deserialize received data!"));
			}

		}
	});
	return request->ProcessRequest();
}
