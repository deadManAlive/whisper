<script lang="ts">
    import { marked } from 'marked';
    import { onMount } from 'svelte';

    let websocket: WebSocket;

    type Message = {
        username: string;
        message: string;
    };

    let new_message = "";

    let message_list: Message[] = [];

    function addMessage() {
        let clean_msg = new_message.trim();
        if (clean_msg === "") {
            return;
        }

        websocket.send(clean_msg);
        new_message = "";
    }

    const keepAlive = () => {
        const gateway = `ws://${window.location.host}/ws`;
        console.log("Opening websocket connection to " + gateway);
        websocket = new WebSocket(gateway);
        websocket.onerror = (event) => {
            console.log(`websocket: error connecting to ${gateway}`);
        };
        websocket.onmessage = (event) => {
            console.log(event.data);
            let data: Message = JSON.parse(event.data);
            console.log(data);
            message_list = [{username: data.username, message: data.message}, ...message_list]
        };
        websocket.onclose = () => {
            setTimeout(keepAlive, 2000);
        };
    };

    onMount(keepAlive);

</script>

<br/>
{#each message_list as msg}
    <div class="message">
        <div style="padding: 5%;">
            <strong class="{msg.username == 'self' ? 'self' : ''}">{msg.username == "self" ? "You" : msg.username}</strong>
            {@html marked(msg.message)}
        </div>
    </div>
<br/>
{/each}
<div style="height: 20%;">

</div>
<div class="flex-container footer">
    <textarea class="messagefield" bind:value={new_message} placeholder="Write message..."></textarea>
    <button class="sendbutton" on:click={addMessage}>Send</button>
</div>

<!-- <button on:click={() => {
    console.log(message_list);
    console.log(new_message);
}}>DEBUG</button> -->

<style>
    .message {
        display: flex;
        flex-direction: column;
        align-items: center;
        /* box-shadow: 4px 6px #888888; */
        border-radius: 8px;
        outline: auto;
    }
	.flex-container {
        display: flex;
        flex-direction: column;
        align-items: center;
    }
    .footer {
        position: fixed;
        bottom: 0;
        width: 100%;
        align-items: center;
        height: 20%;
    }
    .messagefield {
        width: inherit;
        border-radius: 8px;
    }
    .sendbutton {
        width: inherit;
        border-radius: 8px;
    }
    .self {
        color: red;
    }
</style>