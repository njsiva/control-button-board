<div class="control-button-board mod-pedal">
    <div mod-role="drag-handle" class="mod-drag-handle"></div>
    <div class="mod-plugin-author"><h1>{{brand}}</h1></div>
    <div class="mod-plugin-name"><h1>{{label}}</h1></div>
	<div class="mod-toggle-group">
		<div class="mod-light on" mod-role="bypass-light"></div>
        <div class="mod-control-group">
            <div class="mod-control-image" mod-role="input-control-port" mod-port-symbol="toggle11"></div>
            <div class="mod-control-image" mod-role="input-control-port" mod-port-symbol="toggle12"></div>
        </div>
        <div class="mod-control-group">
            <div class="mod-control-image" mod-role="input-control-port" mod-port-symbol="toggle21"></div>
            <div class="mod-control-image" mod-role="input-control-port" mod-port-symbol="toggle22"></div>
        </div>
        <div class="mod-control-group">
            <div class="mod-control-image" mod-role="input-control-port" mod-port-symbol="toggle31"></div>
            <div class="mod-control-image" mod-role="input-control-port" mod-port-symbol="toggle32"></div>
        </div>
        <div class="mod-control-group">
            <div class="mod-control-image" mod-role="input-control-port" mod-port-symbol="toggle41"></div>
            <div class="mod-control-image" mod-role="input-control-port" mod-port-symbol="toggle42"></div>
        </div>
        <div class="mod-control-group">
            <div class="mod-control-image" mod-role="input-control-port" mod-port-symbol="toggle51"></div>
            <div class="mod-control-image" mod-role="input-control-port" mod-port-symbol="toggle52"></div>
        </div>
	</div>
    <div class="mod-pedal-output">
        {{#effect.ports.cv.output}}
        <div class="mod-output mod-output-disconnected" title="{{name}}"
             mod-role="output-cv-port" mod-port-symbol="{{symbol}}">
            <div class="mod-pedal-output-image"></div>
        </div>
        {{/effect.ports.cv.output}}
    </div>
</div>
