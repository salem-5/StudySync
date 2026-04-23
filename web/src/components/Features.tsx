import ScrollReveal from "./ScrollReveal";

export default function Features() {
    const featuresList = [
        { icon: "ph-squares-four", title: "Dashboard", text: "A unified command center. Access your pinned study groups and track your pending tasks without switching tabs." },
        { icon: "ph-timer", title: "Focus Session", text: "Stay in the flow. Use the built-in timer system to maintain productivity during intensive study blocks." },
        { icon: "ph-users-three", title: "Study Groups", text: "Real-time teamwork. Assign tasks to specific members and stay synchronized via an integrated live chat." },
        { icon: "ph-robot", title: "AI Tutor", text: "Help when you need it. Chat with an AI assistant to get clarification on complex academic subjects instantly." },
        { icon: "ph-globe-hemisphere-west", title: "Multilingual", text: "Study in your preferred language. Fully translated into Arabic, English, and French natively, with more on the way." },
        { icon: "ph-cloud-check", title: "Public Server", text: "Skip the backend setup process entirely. Connect straight to our official public server for an instant study environment." }
    ];

    return (
        <section className="features-grid" id="features">
            {featuresList.map((feature, index) => (
                <ScrollReveal key={index} className="feature-card">
                    <i className={`ph ${feature.icon} feature-icon`}></i>
                    <h3>{feature.title}</h3>
                    <p>{feature.text}</p>
                </ScrollReveal>
            ))}
        </section>
    );
}