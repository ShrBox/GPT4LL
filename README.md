# GPT4LL

Chat with GPT in Bedrock Dedicated Server with LiteLoaderBDS

## Usage

`/chatgpt chat` Start chat with GPT  
`/chatgpt renew` Renew the global chat(Admin)

## Configuration file

Path: `plugins/GPT4LL/config.json`

```jsonc
{
    "apikey": "", // Your openai api key
    "model": "gpt-3.5-turbo", // The model which will use
    "prompt": "You are a AI helper in the Minecraft Bedrock Edition server", // Initialize prompt
    "proxy": "http://127.0.0.1:7890" // Proxy server
}
```

## Credit

[openai-cpp](https://github.com/olrea/openai-cpp)